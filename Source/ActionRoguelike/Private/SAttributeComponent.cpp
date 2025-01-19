// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"



// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	MaxHealth = 100;
	CurrentHealth = MaxHealth;
}

bool USAttributeComponent::ApplyHealthChange(float Delta, AActor* InstigatorActor)
{
	float OldHealth = CurrentHealth;

	CurrentHealth = FMath::Clamp(CurrentHealth + Delta, 0.0f, MaxHealth);

	float ActualDelta = CurrentHealth - OldHealth;
	
	OnHealthChanged.Broadcast(InstigatorActor, this, CurrentHealth, Delta);

	return ActualDelta !=0;
}

float USAttributeComponent::GetMaxHealth() const
{
	return MaxHealth;
}

float USAttributeComponent::GetCurrentHealth() const
{
	return CurrentHealth;
}

bool USAttributeComponent::IsFullHealth() const
{
	return CurrentHealth == MaxHealth;
}

bool USAttributeComponent::IsAlive() const
{
	return CurrentHealth > 0.0;
}

float USAttributeComponent::GetHealthPercent() const
{
	float _healtPercent = (CurrentHealth * 100) / MaxHealth;
	return _healtPercent;
}

USAttributeComponent* USAttributeComponent::GetAttributes(AActor* FromActor)
{
	if(FromActor)
	{
		return Cast<USAttributeComponent>(FromActor->GetComponentByClass(USAttributeComponent::StaticClass()));
	}

	return nullptr;
}

bool USAttributeComponent::IsActorAlive(AActor* Actor)
{
	USAttributeComponent* _attributeComp = GetAttributes(Actor);

	if(_attributeComp)
	{
		return _attributeComp->IsAlive();
	}
	return false;
}
