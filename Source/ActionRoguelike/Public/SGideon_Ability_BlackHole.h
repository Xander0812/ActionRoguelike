// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SProjectileBaseClass.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "SGideon_Ability_BlackHole.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASGideon_Ability_BlackHole : public ASProjectileBaseClass
{
	GENERATED_BODY()
	

protected:
	ASGideon_Ability_BlackHole();

	void DestroyOnAnimationEnd();

	/* Timer of projectile flight time */
	FTimerHandle TimerHandle_FlyTime;

public:
	
	UPROPERTY(VisibleAnywhere)
	URadialForceComponent* RadialForceComp;

	void DeactivateProjectile() override;

	void ActivateProjectile() override;

	/* Basic on hit function */
	void OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	void BeginPlay() override;
};
