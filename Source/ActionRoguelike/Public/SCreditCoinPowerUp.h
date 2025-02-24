// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SInteractablePowerUp.h"
#include "SCreditCoinPowerUp.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASCreditCoinPowerUp : public ASInteractablePowerUp
{
	GENERATED_BODY()

	void Interact_Implementation(APawn* InstigatorPawn) override;

public:

	UPROPERTY(EditDefaultsOnly, Category = "Credits")
	float CreditsRewardAmount;

};
