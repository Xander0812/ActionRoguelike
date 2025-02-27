// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "SSaveGame.h"
#include "SMonsterData.h"
#include "SGameModeBase.generated.h"



class UEnvQuery;
class UCurveFloat;

/*DataTable Row for spawning monsters in game mode*/
USTRUCT(BlueprintType)
struct FMonsterInfoRow : public FTableRowBase
{
	GENERATED_BODY()

public:

	FMonsterInfoRow()
	{
		Weight = 1.f;
		SpawnCost = 5.f;
		KillReward = 20.f;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USMonsterData* MonsterData;

	/* Relative chance to pick this monster. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Weight;

	/* Points requiert by gamemode to spawn this unit. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnCost;

	/* Amount of credits awarded to killer of this unit. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float KillReward;
};

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:

	FString SaveSlotName;

	UPROPERTY()
	USSaveGame* CurrentSaveGame;

	/*All available monsters*/
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UDataTable* MonsterTable;

	//UPROPERTY(EditDefaultsOnly, Category = "AI")
	//TSubclassOf<AActor> MinionClass;

	UPROPERTY(EditDefaultsOnly, Category = "PowerUp")
	TSubclassOf<AActor> CoinPowerUpClass;

	UPROPERTY(EditDefaultsOnly, Category = "PowerUp")
	TSubclassOf<AActor> HealthPowerUpClass;

	/*Rules of setting up pissible bot spawn locations*/
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UEnvQuery* SpawnBotQuery;

	UPROPERTY(EditDefaultsOnly, Category = "PowerUp")
	UEnvQuery* SpawnPowerUpQuery;

	/*Curve for custom updating gamemode dificulty*/
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UCurveFloat* DifficultyCurve;

	FTimerHandle TimerHandle_SpawnBots;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float BotSpawnTimerInterval;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float PlayerRespawnDelay;

	UPROPERTY(EditDefaultsOnly, Category = "Credits")
	float BotKillCreditRewardAmount;

	UPROPERTY(EditDefaultsOnly, Category = "PowerUp")
	int PowerUpsToSpawnAmount;

	UFUNCTION()
	void SpawnBotTimerElapsed();

	UFUNCTION()
	void SpawnPowerUp();

	UFUNCTION()
	void OnBotSpawnQueryComplited(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UFUNCTION()
	void OnPowerUpSpawnQueryComplited(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UFUNCTION()
	void RespawnPlayerElapsed(AController* Controller);

public:

	virtual void OnActorKilled(AActor* VictimActor, AActor* Killer);

	ASGameModeBase();

	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	virtual void StartPlay() override;

	UFUNCTION(Exec)
	void KillAll();

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void WriteSaveGame();

	void LoadSaveGame();
};
