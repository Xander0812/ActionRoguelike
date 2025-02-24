// Fill out your copyright notice in the Description page of Project Settings.

#include "SAttributeComponent.h"
#include "SGameModeBase.h"
#include "Net/UnrealNetwork.h"

static TAutoConsoleVariable<float> CVarDamageMultiplyer(TEXT("ar.DamageMultiplier"), 1.0f, TEXT("Global Damage Modifier for Attribute Component"), ECVF_Cheat);

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	MaxHealth = 100;
	CurrentHealth = MaxHealth;

	MaxRage = 30;
	CurrentRage = 0;
	RageGainPerHit = 1;

	SetIsReplicatedByDefault(true);
}

bool USAttributeComponent::Kill(AActor* InstigatorActor)
{
	return ApplyHealthChange(-GetMaxHealth(), InstigatorActor);
}

bool USAttributeComponent::ApplyHealthChange(float Delta, AActor* InstigatorActor)
{
	if(!GetOwner()->CanBeDamaged() && Delta < 0.f)
	{
		return false;
	}

	if(Delta < 0.0f)
	{
		float _damageMultiplier = CVarDamageMultiplyer.GetValueOnGameThread();

		Delta *= _damageMultiplier;
	}

	float _oldHealth = CurrentHealth;
	float _newHealth = FMath::Clamp(CurrentHealth + Delta, 0.0f, MaxHealth);
	float _actualDelta = _newHealth - _oldHealth;

	//is server?
	if(GetOwner()->HasAuthority())
	{
		CurrentHealth = _newHealth;

		if (_actualDelta != 0.f)
		{
			MulticastHealthChanged(InstigatorActor, CurrentHealth, Delta);
		}

		//Died
		if (_actualDelta < 0.f && !IsAlive())
		{
			ASGameModeBase* GM = GetWorld()->GetAuthGameMode<ASGameModeBase>();
			if (GM)
			{
				GM->OnActorKilled(GetOwner(), InstigatorActor);
			}
		}

		ApplyRageChange(RageGainPerHit, InstigatorActor);

	}

	return _actualDelta !=0;
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

bool USAttributeComponent::IsActorAlive(AActor* Actor)
{
	USAttributeComponent* _actorAttributeComp = GetAttributes(Actor);

	if(_actorAttributeComp)
	{
		return _actorAttributeComp->IsAlive();
	}
	return false;
}


/*Rage*/


bool USAttributeComponent::ApplyRageChange(float Delta, AActor* InstigatorActor)
{
	if(CurrentRage + Delta <= 0)
	{
		return false;
	}

	float _oldRage = CurrentRage;

	CurrentRage = FMath::Clamp(CurrentRage + Delta, 0.0f, MaxRage);

	float _actualDelta = CurrentRage - _oldRage;

	//OnRageChanged.Broadcast(InstigatorActor, this, CurrentRage, Delta);
	MulticastRageChanged(InstigatorActor, CurrentRage, Delta);

	return _actualDelta != 0;
}

float USAttributeComponent::GetMaxRage() const
{
	return MaxRage;
}

float USAttributeComponent::GetCurrentRage() const
{
	return CurrentRage;
}

bool USAttributeComponent::IsFullRage() const
{
	return CurrentRage == MaxRage;
}

USAttributeComponent* USAttributeComponent::GetAttributes(AActor* FromActor)
{
	if(FromActor)
	{
		return Cast<USAttributeComponent>(FromActor->GetComponentByClass(USAttributeComponent::StaticClass()));
	}

	return nullptr;
}

/*Replication*/

void USAttributeComponent::MulticastHealthChanged_Implementation(AActor* InstigatorActor, float NewHealth, float Delta)
{
	OnHealthChanged.Broadcast(InstigatorActor, this, NewHealth, Delta);
}

void USAttributeComponent::MulticastRageChanged_Implementation(AActor* InstigatorActor, float NewRage, float Delta)
{
	OnRageChanged.Broadcast(InstigatorActor, this, NewRage, Delta);
}

void USAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USAttributeComponent, CurrentHealth);
	DOREPLIFETIME_CONDITION(USAttributeComponent, MaxHealth, COND_OwnerOnly)

	DOREPLIFETIME(USAttributeComponent, CurrentRage);
	DOREPLIFETIME_CONDITION(USAttributeComponent, MaxRage, COND_OwnerOnly)

}