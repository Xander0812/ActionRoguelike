// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthPotion.h"

void ASHealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	//We heal the object that interacts with this buff

	if(InstigatorPawn)
	{
		USAttributeComponent* _attributeComp = InstigatorPawn->GetComponentByClass<USAttributeComponent>();

		if(!_attributeComp->IsFullHealth())
		{
			if(_attributeComp->GetCurrentHealth() + HealingAmount > _attributeComp->GetMaxHealth())
			{
				_attributeComp->ApplyHealthChange(_attributeComp->GetMaxHealth() - _attributeComp->GetCurrentHealth(), this);
			}
			else
			{
				_attributeComp->ApplyHealthChange(HealingAmount, this);
			}
			Super::Interact_Implementation(InstigatorPawn);
		}
	}
}
