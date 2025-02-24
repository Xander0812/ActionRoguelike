// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SInteractablePowerUp.h"
#include "SActionComponent.h"
#include "SAction.h"
#include "SAbilityPowerUp.generated.h"


/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASAbilityPowerUp : public ASInteractablePowerUp
{
	GENERATED_BODY()
	
	void Interact_Implementation(APawn* InstigatorPawn) override;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<USAction> ActionAbilityClass;
};
