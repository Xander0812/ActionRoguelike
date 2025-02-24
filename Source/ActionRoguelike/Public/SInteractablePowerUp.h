// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGameplayInterface.h"
#include "SAttributeComponent.h"
#include "Components/SphereComponent.h"

#include "SInteractablePowerUp.generated.h"


UCLASS()
class ACTIONROGUELIKE_API ASInteractablePowerUp : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASInteractablePowerUp();

	virtual void Interact_Implementation(APawn* InstigatorPawn);

	UPROPERTY(EditAnywhere)
	float ReactivationTime;

protected:

	UPROPERTY(ReplicatedUsing = "OnRep_UsePowerUp", BlueprintReadOnly) //RepNotify == ReplicatedUsing
	bool bInteractable;

	UFUNCTION()
	void OnRep_UsePowerUp();

	/*Turn off Buff interaction on use*/
	void Deactivate();

	/*Return Buff interaction when we need to*/
	void Reactivate();

	/*Set New State: true = turned on, false = turned off*/
	void SetPowerUpState();

	/* Timer of power up reactivation */
	FTimerHandle TimerHandle_Reactivation;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	UStaticMeshComponent* BaseMeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	USphereComponent* SphereComp;
};
