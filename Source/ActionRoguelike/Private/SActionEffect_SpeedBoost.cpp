// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionEffect_SpeedBoost.h"
#include "SAttributeComponent.h"
#include "SActionComponent.h"

USActionEffect_SpeedBoost::USActionEffect_SpeedBoost()
{
	MaxDuration = 6;
	SpeedBoostFraction = 0.2f;
}

void USActionEffect_SpeedBoost::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	USAttributeComponent* _attributes = USAttributeComponent::GetAttributes(GetOwningComponent()->GetOwner());

	_attributes->ActivateSpeedBoost(SpeedBoostFraction);
}

void USActionEffect_SpeedBoost::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);

	USAttributeComponent* _attributes = USAttributeComponent::GetAttributes(GetOwningComponent()->GetOwner());

	_attributes->DeactivateSpeedBoost();
}
