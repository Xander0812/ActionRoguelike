// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"
#include <Net/UnrealNetwork.h>
#include <../ActionRoguelike.h>

ASPlayerState::ASPlayerState()
{
	//SetIsReplicatedByDefault(true);
	SetReplicates(true);
}

bool ASPlayerState::AddCredits(int32 Delta, AActor* InstigatorActor)
{
	if(!ensure(Delta > 0.0f))
	{
		return false;
	}
	CreditAmount += Delta;

	//LogOnScreen(this, FString::FromInt(Delta), FColor::Green);

	OnRep_Credits(CreditAmount - Delta);
	//MulticastCreditsChanged(InstigatorActor, CreditAmount, Delta);

	return true;
}

bool ASPlayerState::RemoveCredits(int32 Delta, AActor* InstigatorActor)
{
	if (!ensure(Delta > 0.0f))
	{
		return false;
	}

	if(CreditAmount < Delta)
	{
		return false;
	}
	
	CreditAmount -= Delta;

	OnRep_Credits(CreditAmount + Delta);
	//MulticastCreditsChanged(InstigatorActor, CreditAmount, Delta);

	return true;
}

int32 ASPlayerState::GetCurrentCredits() const
{
	return CreditAmount;
}

void ASPlayerState::OnRep_Credits(int32 OldCredits)
{
	OnCreditsChanged.Broadcast(this, CreditAmount, CreditAmount - OldCredits);
}

void ASPlayerState::SavePlayerState_Implementation(USSaveGame* SaveObject)
{
	if(SaveObject)
	{
		SaveObject->Credits = CreditAmount;
	}
}

void ASPlayerState::LoadPlayerState_Implementation(USSaveGame* SaveObject)
{
	if (SaveObject)
	{
		AddCredits(SaveObject->Credits, this);
	}
}

/*
void ASPlayerState::MulticastCreditsChanged_Implementation(AActor* InstigatorActor, float NewCreditAmount, float Delta)
{
	OnCreditsChanged.Broadcast(this, NewCreditAmount, Delta);
}
*/

void ASPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerState, CreditAmount);
}