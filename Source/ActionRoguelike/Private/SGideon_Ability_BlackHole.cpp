// Fill out your copyright notice in the Description page of Project Settings.


#include "SGideon_Ability_BlackHole.h"
#include "SGameplayFunctionLibrary.h"


ASGideon_Ability_BlackHole::ASGideon_Ability_BlackHole()
{
	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));

	MovementComp->InitialSpeed = 1200;

	RadialForceComp->SetupAttachment(SphereComp);
	RadialForceComp->ForceStrength = -2000;
	RadialForceComp->bAutoActivate = false;
	RadialForceComp->Radius = 700;

	//Set OnComponentHit action event
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASGideon_Ability_BlackHole::OnComponentOverlap);
}

void ASGideon_Ability_BlackHole::BeginPlay()
{
	Super::BeginPlay();

	//Start animation timer. We play particle effect after timer runs out
	GetWorldTimerManager().SetTimer(TimerHandle_FlyTime, this, &ASGideon_Ability_BlackHole::DestroyOnAnimationEnd, 5.0f);
}

void ASGideon_Ability_BlackHole::DeactivateProjectile()
{
	Super::DeactivateProjectile();
	RadialForceComp->Deactivate();
}

void ASGideon_Ability_BlackHole::ActivateProjectile()
{
	Super::ActivateProjectile();
	RadialForceComp->Activate();
}

void ASGideon_Ability_BlackHole::OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnComponentOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	//Custom black hole ability projectile overlap behavior
	//We just destroy object that are too close
	if (OtherActor == GetInstigator())
	{
		return;
	}

	if(OtherActor && OtherComp->GetCollisionObjectType() == ECollisionChannel::ECC_PhysicsBody)
	{
		OtherActor->Destroy();
	}

	USGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), OtherActor, 999, SweepResult);
}

void ASGideon_Ability_BlackHole::DestroyOnAnimationEnd()
{
	DeactivateProjectile();
}