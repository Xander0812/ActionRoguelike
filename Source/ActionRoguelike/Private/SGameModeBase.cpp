// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "AI/SAICharacter.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"
#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "SGameplayInterface.h"
#include <Serialization/ObjectAndNameAsStringProxyArchive.h>
#include <../ActionRoguelike.h>
#include <Engine/AssetManager.h>
#include "SSaveGameSubsystem.h"


static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("ar.SpawnBots"), true, TEXT("Enable, spawning of bots via timer"), ECVF_Cheat);

ASGameModeBase::ASGameModeBase()
{
	BotSpawnTimerInterval = 2.0f;

	PlayerStateClass = ASPlayerState::StaticClass();
}


void ASGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// (Save/Load logic moved into new SaveGameSubsystem)
	USSaveGameSubsystem* _sg = GetGameInstance()->GetSubsystem<USSaveGameSubsystem>();

	// Optional slot name (Falls back to slot specified in SaveGameSettings class/INI otherwise)
	//FString _selectedSaveSlot = UGameplayStatics::ParseOption(Options, "SaveGame");
	_sg->LoadSaveGame();
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	SpawnPowerUp();
	//Continuous timer to spawn more bots
	//Actual amount of bots and whether it's allowed to spawn determined by spawn logic later

	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &ASGameModeBase::SpawnBotTimerElapsed, BotSpawnTimerInterval, true);
}

void ASGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// Calling Before Super:: so we set variables before 'beginplayingstate' is called in PlayerController (which is where we instantiate UI)
	USSaveGameSubsystem* _sg = GetGameInstance()->GetSubsystem<USSaveGameSubsystem>();
	_sg->HandleStartingNewPlayer(NewPlayer);

	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	// Now we're ready to override spawn location
	// Alternatively we could override core spawn location to use store locations immediately (skipping the whole 'find player start' logic)
	_sg->OverrideSpawnTransform(NewPlayer);
}

void ASGameModeBase::KillAll()
{
	int32 _nrOfAliveBots = 0;
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* _bot = *It;

		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(_bot);
		if (AttributeComp && AttributeComp->IsAlive())
		{
			AttributeComp->Kill(this); //@fixme: pass in player? for kill credit
		}
	}
}

void ASGameModeBase::SpawnBotTimerElapsed()
{
	//Check if we have room to spawn new Bots outside player's visibility
	//Bots maximum is controlled via Difficulty curve

	if(!CVarSpawnBots.GetValueOnGameThread())
	{
		UE_LOG(LogTemp, Warning, TEXT("Bot spawning disabled via cvar 'CVarSpawnBots'"));
		return;
	}

	int32 _nrOfAliveBots = 0;
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* _bot = *It;

		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(_bot);
		if (AttributeComp && AttributeComp->IsAlive())
		{
			_nrOfAliveBots++;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Found %i alive bots."), _nrOfAliveBots);

	float _maxBotCount = 10.0f;

	if (DifficultyCurve)
	{
		_maxBotCount = DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}

	if (_nrOfAliveBots >= _maxBotCount)
	{
		UE_LOG(LogTemp, Log, TEXT("At maximum capacity. Skipping bot spawn."));
		return;
	}

	UEnvQueryInstanceBlueprintWrapper* _queryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);
	if(ensure(_queryInstance))
	{
		_queryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnBotSpawnQueryComplited);
	}
}

void ASGameModeBase::OnBotSpawnQueryComplited(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	//Called few frames after running EQSQuery
	//Spawning bots on collected locations 

	if(QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS Query Failed!"));
		return;
	}

	TArray<FVector> _locations = QueryInstance->GetResultsAsLocations();

	if(_locations.Num() > 0)
	{
		if (MonsterTable)
		{
			TArray<FMonsterInfoRow*> _rows;
			MonsterTable->GetAllRows("", _rows);

			//Get Random Enemy
			int32 _randomIndex = FMath::RandRange(0, _rows.Num() - 1);
			FMonsterInfoRow* _selectedRow = _rows[_randomIndex];

			UAssetManager* _assetManager = UAssetManager::GetIfValid();
			if(_assetManager)
			{
				//LogOnScreen(this, "Loading monster...", FColor::Green);

				TArray<FName> _bundles;
				FStreamableDelegate _streamDelegate = FStreamableDelegate::CreateUObject(this, &ASGameModeBase::OnMonsterLoaded, _selectedRow->MonsterID, _locations[0]);
				_assetManager->LoadPrimaryAsset(_selectedRow->MonsterID, _bundles, _streamDelegate, -1);
			}
		}
	}
}

void ASGameModeBase::OnMonsterLoaded(FPrimaryAssetId LoadedId, FVector SpawnLocation)
{
	//LogOnScreen(this, "Fineshed monster loading!!", FColor::Green);

	FActorSpawnParameters _spawnParams;
	_spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	UAssetManager* _assetManager = UAssetManager::GetIfValid();
	if (_assetManager)
	{
		USMonsterData* _monsterData = Cast<USMonsterData>(_assetManager->GetPrimaryAssetObject(LoadedId));
		if (_monsterData)
		{
			AActor* _newBot = GetWorld()->SpawnActor<AActor>(_monsterData->MonsterClass, SpawnLocation, FRotator::ZeroRotator, _spawnParams);

			if (_newBot)
			{
				//LogOnScreen(this, FString::Printf(TEXT("Spawned enemy: %s (%s)"), *GetNameSafe(_newBot), *GetNameSafe(_monsterData)));

				//Grant special actions/buffs
				USActionComponent* _botActionComp = Cast<USActionComponent>(_newBot->GetComponentByClass(USActionComponent::StaticClass()));
				if (_botActionComp && !_monsterData->Actions.IsEmpty())
				{
					for (TSubclassOf<USAction> _actionClass : _monsterData->Actions)
					{
						_botActionComp->AddAction(_newBot, _actionClass);
					}
				}
			}
		}
	}
}

void ASGameModeBase::SpawnPowerUp()
{
	//run powerUP locations EQS
	UEnvQueryInstanceBlueprintWrapper* _queryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnPowerUpQuery, this, EEnvQueryRunMode::AllMatching, nullptr);
	if (ensure(_queryInstance))
	{
		_queryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnPowerUpSpawnQueryComplited);
	}
}

void ASGameModeBase::OnPowerUpSpawnQueryComplited(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	FActorSpawnParameters _spawnParams;
	_spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	TArray<FVector> _locations = QueryInstance->GetResultsAsLocations();


	//spawn coins
	if (_locations.Num() > 0)
	{
		for(int i = 0; i < PowerUpsToSpawnAmount; i++)
		{
			double _chosenLocation = FMath::FRandRange(0, double(_locations.Num() - 1));

			GetWorld()->SpawnActor<AActor>(CoinPowerUpClass, _locations[_chosenLocation], FRotator::ZeroRotator, _spawnParams);

			_locations.RemoveAt(_chosenLocation);
		}
	}

	//spawn Health potions
	if (_locations.Num() > 0)
	{
		for (int i = 0; i < PowerUpsToSpawnAmount; i++)
		{
			double _chosenLocation = FMath::FRandRange(0, double(_locations.Num() - 1));

			GetWorld()->SpawnActor<AActor>(HealthPowerUpClass, _locations[_chosenLocation], FRotator::ZeroRotator, _spawnParams);

			_locations.RemoveAt(_chosenLocation);
		}
	}
}

void ASGameModeBase::RespawnPlayerElapsed(AController* Controller)
{
	if(ensure(Controller))
	{
		Controller->UnPossess();

		RestartPlayer(Controller);
	}
}

void ASGameModeBase::OnActorKilled(AActor* VictimActor, AActor* Killer)
{
	if (VictimActor->IsA(ASAICharacter::StaticClass()))
	{
		APawn* _killerPawn = Cast<APawn>(Killer);
		if (_killerPawn) {
			ASPlayerState* _playerState = _killerPawn->GetPlayerState<ASPlayerState>();

			if (_playerState)
			{
				//Give credits for bot kill
				_playerState->AddCredits(BotKillCreditRewardAmount);
			}
		}
	}

	if (VictimActor->IsA(ASCharacter::StaticClass()))
	{
		ASCharacter* _playerCharacter = Cast<ASCharacter>(VictimActor);
		if (_playerCharacter)
		{
			FTimerHandle _timerHandle_RespawnDelay;

			FTimerDelegate _timerDelegate;
			_timerDelegate.BindUFunction(this, "RespawnPlayerElapsed", _playerCharacter->GetController());

			GetWorldTimerManager().SetTimer(_timerHandle_RespawnDelay, _timerDelegate, PlayerRespawnDelay, false);

			ASPlayerState* PS = _playerCharacter->GetPlayerState<ASPlayerState>();
			if (PS)
			{
				PS->UpdatePersonalRecord(GetWorld()->TimeSeconds);
			}

			USSaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<USSaveGameSubsystem>();
			// Immediately auto save on death
			SG->WriteSaveGame();
		}

		UE_LOG(LogTemp, Log, TEXT("OnActorKilled: Victim: %s, Killer: %s"), *GetNameSafe(VictimActor), *GetNameSafe(Killer));
	}
}
