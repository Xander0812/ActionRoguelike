// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "AI/SAICharacter.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"
#include "SAttributeComponent.h"



ASGameModeBase::ASGameModeBase()
{
	SpawnTimerInterval = 2.0f;
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	//Continuous timer to spawn more bots
	//Actual amount of bots and whether it's allowed to spawn determined by spawn logic later
	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &ASGameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);
}

void ASGameModeBase::SpawnBotTimerElapsed()
{
	//Check if we have room to spawn new Bots outside player's visibility
	//Bots maximum is controlled via Difficulty curve

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
		_queryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnQueryComplited);
	}
}

void ASGameModeBase::OnQueryComplited(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
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
		GetWorld()->SpawnActor<AActor>(MinionClass, _locations[0], FRotator::ZeroRotator, _spawnParams);

		DrawDebugSphere(GetWorld(), _locations[0], 50.f, 20, FColor::Blue, false, 60);
	}
}
