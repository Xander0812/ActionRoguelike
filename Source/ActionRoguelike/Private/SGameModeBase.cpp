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


static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("ar.SpawnBots"), true, TEXT("Enable, spawning of bots via timer"), ECVF_Cheat);


void ASGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	LoadSaveGame();
}

void ASGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	ASPlayerState* _playerState = NewPlayer->GetPlayerState<ASPlayerState>();

	if(_playerState)
	{
		_playerState->LoadPlayerState(CurrentSaveGame);
	}
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
}

ASGameModeBase::ASGameModeBase()
{
	BotSpawnTimerInterval = 2.0f;

	SaveSlotName = "SaveGame01";
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	SpawnPowerUp();
	//Continuous timer to spawn more bots
	//Actual amount of bots and whether it's allowed to spawn determined by spawn logic later

	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &ASGameModeBase::SpawnBotTimerElapsed, BotSpawnTimerInterval, true);
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

	FActorSpawnParameters _spawnParams;
	_spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	TArray<FVector> _locations = QueryInstance->GetResultsAsLocations();

	if(_locations.Num() > 0)
	{
		if(MonsterTable)
		{
			TArray<FMonsterInfoRow*> _rows;
			MonsterTable->GetAllRows("", _rows);

			//Get Random Enemy
			int32 _randomIndex = FMath::RandRange(0, _rows.Num() - 1);
			FMonsterInfoRow* _selectedRow = _rows[_randomIndex];

			AActor* _newBot = GetWorld()->SpawnActor<AActor>(_selectedRow->MonsterData->MonsterClass, _locations[0], FRotator::ZeroRotator, _spawnParams);

			if(_newBot)
			{
				LogOnScreen(this, FString::Printf(TEXT("Spawned enemy: %s (%s)"), *GetNameSafe(_newBot), *GetNameSafe(_selectedRow->MonsterData)));

				//Grant special actions/buffs
				USActionComponent* _botActionComp = Cast<USActionComponent>(_newBot->GetComponentByClass(USActionComponent::StaticClass()));
				if(_botActionComp)
				{
					for(TSubclassOf<USAction> _actionClass : _selectedRow->MonsterData->Actions)
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
				_playerState->AddCredits(BotKillCreditRewardAmount, VictimActor);
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
		}

		UE_LOG(LogTemp, Log, TEXT("OnActorKilled: Victim: %s, Killer: %s"), *GetNameSafe(VictimActor), *GetNameSafe(Killer));
	}
}


//Saves


void ASGameModeBase::WriteSaveGame()
{
	// Iterate all player states, we don't have proper ID to match yet (requiers Steam or EQS)
	for(int32 i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		ASPlayerState* _playerState = Cast<ASPlayerState>(GameState->PlayerArray[i]);
		if(_playerState)
		{
			_playerState->SavePlayerState(CurrentSaveGame);

			break; //single player only at this point
		}
	}

	CurrentSaveGame->SavedActors.Empty();

	//Itirate the entire world of actors
	for(FActorIterator _it(GetWorld()); _it; ++_it)
	{
		AActor* _actor = *_it;
		//only interested in our 'gameplay actors'
		if(!_actor->Implements<USGameplayInterface>())
		{
			continue;
		}

		FActorSaveData _actorData;
		_actorData.ActorName = _actor->GetName();
		_actorData.Transform = _actor->GetActorTransform();

		//Pass in the array to fill with data from Actor
		FMemoryWriter _memWriter(_actorData.ByteData);
		FObjectAndNameAsStringProxyArchive _ar(_memWriter, true);

		// Find ONLY viriables with UPROPERTY(SaveGame)
		_ar.ArIsSaveGame = true;

		//Converts Actpor's SaveGame UPROPERTIES to binary array
		_actor->Serialize(_ar);

		CurrentSaveGame->SavedActors.Add(_actorData);
	}

	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SaveSlotName, 0);
}

void ASGameModeBase::LoadSaveGame()
{
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
		if (CurrentSaveGame == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load SaveGame data."));
			return;
		}

		UE_LOG(LogTemp, Log, TEXT("loaded SaveGame data."));

		for (FActorIterator _it(GetWorld()); _it; ++_it)
		{
			AActor* _actor = *_it;
			//only interested in our 'gameplay actors'
			if (!_actor->Implements<USGameplayInterface>())
			{
				continue;
			}

			for (FActorSaveData _actorData : CurrentSaveGame->SavedActors)
			{
				if (_actorData.ActorName == _actor->GetName())
				{
					_actor->SetActorTransform(_actorData.Transform);

					//Get filled array from Actor SaveGame
					FMemoryReader _memReader(_actorData.ByteData);
					FObjectAndNameAsStringProxyArchive _ar(_memReader, true);
					_ar.ArIsSaveGame = true;

					//Converts binary array back to actor's variables
					_actor->Serialize(_ar);

					ISGameplayInterface::Execute_OnActorLoaded(_actor);

					break;
				}
			}
		}
	}
	else
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::CreateSaveGameObject(USSaveGame::StaticClass()));

		UE_LOG(LogTemp, Log, TEXT("Created new SaveGame data."));
	}

	
}
