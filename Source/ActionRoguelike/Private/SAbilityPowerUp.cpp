// Fill out your copyright notice in the Description page of Project Settings.


#include "SAbilityPowerUp.h"

void ASAbilityPowerUp::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!ensure(InstigatorPawn && ActionAbilityClass))
	{
		return;
	}


	USActionComponent* _actorActionsComp = USActionComponent::GetActionsComponent(InstigatorPawn);

	if (_actorActionsComp) //&& !_actorActionsComp->HasAction(Cast<USAction>(ActionAbilityClass)))
	{
		if(_actorActionsComp->HasAction(ActionAbilityClass))
		{
			return;
		}

		_actorActionsComp->AddAction(InstigatorPawn, ActionAbilityClass);

		Super::Interact_Implementation(InstigatorPawn);
	}
}
