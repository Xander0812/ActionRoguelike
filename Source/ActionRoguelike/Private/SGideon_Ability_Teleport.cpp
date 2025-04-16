// Fill out your copyright notice in the Description page of Project Settings.


#include "SGideon_Ability_Teleport.h"

ASGideon_Ability_Teleport::ASGideon_Ability_Teleport()
{
	MovementComp->InitialSpeed = 2000;

	OverlapLocation = FVector(0, 0, 0);

	//Set OnComponentHit action event
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASGideon_Ability_Teleport::OnComponentOverlap);
}

void ASGideon_Ability_Teleport::BeginPlay()
{
	Super::BeginPlay();

	//Start animation timer. We play particle effect after timer runs out
	//GetWorldTimerManager().SetTimer(TimerHandle_FlyTime, this, &ASGideon_Ability_Teleport::PlayTeleportParticle, 0.2f);
}

void ASGideon_Ability_Teleport::OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnComponentOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if(OtherActor && !OtherActor->IsA(ASProjectileBaseClass::StaticClass()))
	{
		OverlapLocation = SweepResult.ImpactPoint;
		//Call function that should've been called after timer
		&ASGideon_Ability_Teleport::PlayTeleportParticle;
	}

}

void ASGideon_Ability_Teleport::PlayTeleportParticle()
{
	//Stop timer because projectile collided with something before timer went out
	GetWorldTimerManager().ClearTimer(TimerHandle_FlyTime);

	//disable collision so projectile would not generate collision and on hit events
	// 
	//SetActorEnableCollision(false);
	//SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//We stop projectile so we would teleport right at the point it triggered this function
	//MovementComp->StopMovementImmediately();
	//MovementComp->SetVelocityInLocalSpace(FVector(0, 0, 0));

	//Deactivate projectile base visual effect. We don't need it anymore
	//BaseParticleEffect->Deactivate();

	DeactivateProjectile();

	//We spawn Emmiter to play animation of teleportation
	ExtraParticleEffectComp->Activate();
	//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExtraParticleEffect, GetActorLocation(), GetActorRotation(), true);

	//We need to setup another timer so the particle animation would play for some time before teleporting to the point
	GetWorldTimerManager().SetTimer(TimerHandle_Teleport, this, &ASGideon_Ability_Teleport::TriggerTeleport, 0.2f);
}

void ASGideon_Ability_Teleport::DeactivateProjectile()
{
	Super::DeactivateProjectile();
}

void ASGideon_Ability_Teleport::ActivateProjectile()
{
	Super::ActivateProjectile();
	OverlapLocation = FVector(0, 0, 0);
	GetWorldTimerManager().SetTimer(TimerHandle_FlyTime, this, &ASGideon_Ability_Teleport::PlayTeleportParticle, 0.2f);
}

void ASGideon_Ability_Teleport::TriggerTeleport()
{
	//Teleport to projectile location and destroy the projectile
	GetInstigator()->SetActorLocation(OverlapLocation == FVector(0, 0, 0) ? GetActorLocation() : OverlapLocation);
}
