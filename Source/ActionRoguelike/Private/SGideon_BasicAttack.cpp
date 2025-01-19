// Fill out your copyright notice in the Description page of Project Settings.


#include "SGideon_BasicAttack.h"

ASGideon_BasicAttack::ASGideon_BasicAttack()
{
	MovementComp->InitialSpeed = 1500;

	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASProjectileBaseClass::OnComponentOverlap);
}

void ASGideon_BasicAttack::OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnComponentOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	//Deal damage on component over lap if it has attribut component aka has health to deal damage to

	if ((OtherActor) && OtherActor != GetInstigator() && !OtherActor->IsA(ASProjectileBaseClass::StaticClass()))
	{
		USAttributeComponent* _attributeComp = USAttributeComponent::GetAttributes(OtherActor);
		if (_attributeComp)
		{
			_attributeComp->ApplyHealthChange(-ProjectileDamage, GetInstigator());
		}
		Destroy();
	}
}