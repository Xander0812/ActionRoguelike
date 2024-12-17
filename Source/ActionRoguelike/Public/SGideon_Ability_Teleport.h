// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SProjectileBaseClass.h"
#include "SGideon_Ability_Teleport.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASGideon_Ability_Teleport : public ASProjectileBaseClass
{
	GENERATED_BODY()
	
protected:
	ASGideon_Ability_Teleport();
	
	/* Timer of projectile flight time */
	FTimerHandle TimerHandle_FlyTime;

	/* Timer to let paricle play before teleporting the player */
	FTimerHandle TimerHandle_Teleport;

public:

	/* We teleport the player to projectile location */
	void TriggerTeleport();

	/* Spawn particles before teleoirting */
	void PlayTeleportParticle();

	/* Basic on hit function */
	void OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	void BeginPlay() override;
};
