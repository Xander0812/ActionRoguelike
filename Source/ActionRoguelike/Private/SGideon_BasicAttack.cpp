// Fill out your copyright notice in the Description page of Project Settings.


#include "SGideon_BasicAttack.h"
#include "SGameplayFunctionLibrary.h"
#include "SActionComponent.h"


ASGideon_BasicAttack::ASGideon_BasicAttack()
{
	MovementComp->InitialSpeed = 1500;

	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASGideon_BasicAttack::OnComponentOverlap);
}

void ASGideon_BasicAttack::OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Deal damage on component overlap if it has attribut component aka has health to deal damage to

	if (OtherActor && OtherActor != GetInstigator() && !OtherActor->IsA(ASProjectileBaseClass::StaticClass()))
	{
		//FGameplayTag _parryTag = FGameplayTag::RequestGameplayTag("Status.Parrying");

		USActionComponent* _actionComp = Cast<USActionComponent>(OtherActor->GetComponentByClass(USActionComponent::StaticClass()));
		if (_actionComp && _actionComp->ActiveGameplayTags.HasTag(ParryTag))
		{
			MovementComp->Velocity = -MovementComp->Velocity;

			SetInstigator(Cast<APawn>(OtherActor));
			return;
		}

		Super::OnComponentOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

		if (USGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), OtherActor, ProjectileDamage, SweepResult))
		{
			Destroy();

			if(_actionComp && HasAuthority())
			{
				_actionComp->AddAction(GetInstigator(), BurningActionClass);
			}
		}
	}
}