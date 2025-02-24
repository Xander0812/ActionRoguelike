// Fill out your copyright notice in the Description page of Project Settings.


#include "SCreditCoinPowerUp.h"
#include <SPlayerState.h>
#include <Kismet/GameplayStatics.h>

void ASCreditCoinPowerUp::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!ensure(InstigatorPawn))
	{
		return;
	}

	if (!bInteractable)
	{
		return;
	}

	if (ASPlayerState* _playerState = InstigatorPawn->GetPlayerState<ASPlayerState>())
	{
		_playerState->AddCredits(CreditsRewardAmount, InstigatorPawn);

		Super::Interact_Implementation(InstigatorPawn);
	}
}