// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SProjectileBaseClass.h"
#include "SGideon_BasicAttack.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASGideon_BasicAttack : public ASProjectileBaseClass
{
	GENERATED_BODY()
	
protected:
	ASGideon_BasicAttack();

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float ProjectileDamage;

	void OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
};
