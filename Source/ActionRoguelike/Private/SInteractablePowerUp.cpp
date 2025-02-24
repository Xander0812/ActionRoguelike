// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractablePowerUp.h"
#include <Net/UnrealNetwork.h>

// Sets default values
ASInteractablePowerUp::ASInteractablePowerUp()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SphereComp->SetCollisionProfileName("PowerUp");
	SphereComp->SetSphereRadius(100);
	RootComponent = SphereComp;

	BaseMeshComp = CreateDefaultSubobject<UStaticMeshComponent>("BaseMesh");
	BaseMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BaseMeshComp->SetupAttachment(SphereComp);

	ReactivationTime = 10;

	bInteractable = true;

	SetReplicates(true);
}

void ASInteractablePowerUp::Interact_Implementation(APawn* InstigatorPawn)
{
	bInteractable = false;
	OnRep_UsePowerUp();
}

void ASInteractablePowerUp::OnRep_UsePowerUp()
{
	Deactivate();
}

void ASInteractablePowerUp::Deactivate()
{
	SetPowerUpState();

	GetWorldTimerManager().SetTimer(TimerHandle_Reactivation, this, &ASInteractablePowerUp::Reactivate, ReactivationTime);
}

void ASInteractablePowerUp::Reactivate()
{
	bInteractable = true;
	SetPowerUpState();
}

void ASInteractablePowerUp::SetPowerUpState()
{
	RootComponent->SetVisibility(bInteractable, true);
	SetActorEnableCollision(bInteractable);
}

void ASInteractablePowerUp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//when value bLidOpend changed -> send ingormation to all clients
	DOREPLIFETIME(ASInteractablePowerUp, bInteractable);
}
