// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionEffect_Thorns.h"
#include <SAttributeComponent.h>
#include "SActionComponent.h"
#include "SGameplayFunctionLibrary.h"


USActionEffect_Thorns::USActionEffect_Thorns()
{
	ReflectFraction = 0.2f;

	Duration = 0.0f;
	Period = 0.0f;
}

void USActionEffect_Thorns::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	USAttributeComponent* _attributes = USAttributeComponent::GetAttributes(GetOwningComponent()->GetOwner());

	if(_attributes)
	{
		_attributes->OnHealthChanged.AddDynamic(this, &USActionEffect_Thorns::OnHealthChanged);
	}
}

void USActionEffect_Thorns::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);

	USAttributeComponent* _attributes = USAttributeComponent::GetAttributes(GetOwningComponent()->GetOwner());

	if (_attributes)
	{
		_attributes->OnHealthChanged.RemoveDynamic(this, &USActionEffect_Thorns::OnHealthChanged);
	}
}

void USActionEffect_Thorns::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	AActor* _owningActor = GetOwningComponent()->GetOwner();

	//Damage Only
	if(Delta < 0.0f && _owningActor != InstigatorActor)
	{
		//Round to nearest number
		int32 _reflectedAmount = FMath::RoundToInt(Delta * ReflectFraction);

		if(_reflectedAmount == 0)
		{
			return;
		}

		_reflectedAmount = FMath::Abs(_reflectedAmount);

		USGameplayFunctionLibrary::ApplyDamage(_owningActor, InstigatorActor, _reflectedAmount);
	}
}
