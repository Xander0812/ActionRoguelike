// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractablePowerUp.h"

// Sets default values
ASInteractablePowerUp::ASInteractablePowerUp()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SphereComp->SetCollisionProfileName("PowerUp");
	RootComponent = SphereComp;

	BaseMeshComp = CreateDefaultSubobject<UStaticMeshComponent>("BaseMesh");
	BaseMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BaseMeshComp->SetupAttachment(SphereComp);
}

void ASInteractablePowerUp::Interact_Implementation(APawn* InstigatorPawn)
{
	Deactivate();
}

void ASInteractablePowerUp::Deactivate()
{
	SetPowerUpState(false);

	GetWorldTimerManager().SetTimer(TimerHandle_Reactivation, this, &ASInteractablePowerUp::Reactivate, ReactivationTime);
}

void ASInteractablePowerUp::Reactivate()
{
	SetPowerUpState(true);
}

void ASInteractablePowerUp::SetPowerUpState(bool bNewPowerUpState)
{
	RootComponent->SetVisibility(bNewPowerUpState, true);
	SetActorEnableCollision(bNewPowerUpState);
}

