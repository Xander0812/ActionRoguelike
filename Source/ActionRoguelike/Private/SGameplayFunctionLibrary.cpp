// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameplayFunctionLibrary.h"
#include "SAttributeComponent.h"

bool USGameplayFunctionLibrary::ApplyDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount)
{
	USAttributeComponent* _targetAttributeComponent = USAttributeComponent::GetAttributes(TargetActor);

	if(_targetAttributeComponent)
	{
		return _targetAttributeComponent->ApplyHealthChange(-DamageAmount, DamageCauser);
	}

	return false;
}

bool USGameplayFunctionLibrary::ApplyDirectionalDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount, const FHitResult& HitResult)
{
	if(ApplyDamage(DamageCauser, TargetActor, DamageAmount))
	{
		UPrimitiveComponent* _hitComp = HitResult.GetComponent();

		if(_hitComp && _hitComp-> IsSimulatingPhysics(HitResult.BoneName))
		{
			//_impulseDirection = target - origin
			FVector _impulseDirection = HitResult.TraceEnd - HitResult.TraceStart;
			_impulseDirection.Normalize();

			_hitComp->AddImpulseAtLocation(_impulseDirection * 300000.f, HitResult.ImpactNormal, HitResult.BoneName);
		}
		return true;
	}

	return false;
}
