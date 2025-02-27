// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthPotion.h"
#include "SPlayerState.h"

#define LOCTEXT_NAMESPACE "InteractableActors"

void ASHealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	//We heal the object that interacts with this buff

	if (!ensure(InstigatorPawn))
	{
		return;
	}

	if(!bInteractable)
	{
		return;
	}

	USAttributeComponent* _attributeComp = InstigatorPawn->GetComponentByClass<USAttributeComponent>();

	if (_attributeComp && !_attributeComp->IsFullHealth())
	{

		if (ASPlayerState* _playerState = InstigatorPawn->GetPlayerState<ASPlayerState>())
		{

			if (_playerState->RemoveCredits(HealingCreditCost, this) && _attributeComp->ApplyHealthChange(HealingAmount, this))
			{

				//Activated when healed sucsessfully
				Super::Interact_Implementation(InstigatorPawn);

			}
		}
	}
}

FText ASHealthPotion::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	USAttributeComponent* _attributeComp = InstigatorPawn->GetComponentByClass<USAttributeComponent>();

	if (_attributeComp && _attributeComp->IsFullHealth())
	{
		return LOCTEXT("HealthPotion_FullHealthWarning", "Already at full health.");
	}

	return FText::Format(LOCTEXT("HealthPotion_InteractMessage", "Cost {0} Credits. Restores health to maximum."), HealingCreditCost);
}

#undef LOCTEXT_NAMESPACE 