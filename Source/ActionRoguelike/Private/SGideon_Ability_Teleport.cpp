// Fill out your copyright notice in the Description page of Project Settings.


#include "SGideon_Ability_Teleport.h"
#include <Kismet/GameplayStatics.h>

ASGideon_Ability_Teleport::ASGideon_Ability_Teleport()
{
	MovementComp->InitialSpeed = 2000;
}

void ASGideon_Ability_Teleport::BeginPlay()
{
	Super::BeginPlay();

	//Set OnComponentHit action event
	SphereComp->OnComponentHit.AddDynamic(this, &ASGideon_Ability_Teleport::OnHit);

	//Start animation timer. We play particle effect after timer runs out
	GetWorldTimerManager().SetTimer(TimerHandle_FlyTime, this, &ASGideon_Ability_Teleport::PlayTeleportParticle, 0.2f);
}

void ASGideon_Ability_Teleport::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//Stop timer because projectile collided with something before timer went out
	GetWorldTimerManager().ClearTimer(TimerHandle_FlyTime);

	//Call function that should've been called after timer
	&ASGideon_Ability_Teleport::PlayTeleportParticle;
}

void ASGideon_Ability_Teleport::PlayTeleportParticle()
{
	//We stop projectile so we would teleport right at the point it triggered this function
	MovementComp->StopMovementImmediately();

	//Change collision profile so the projectile would not generate collision and on hit events
	SphereComp->SetCollisionProfileName(TEXT("OverlapAll"));

	//Deactivate projectile base visual effect. We don't need it anymore
	EffectComp->Deactivate();

	//We spawn Emmiter to play animation of teleportation
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TeleportEffectComp, GetActorLocation(), GetActorRotation(), true);

	//We need to setup another timer so the particle animation would play for some time before teleporting to the point
	GetWorldTimerManager().SetTimer(TimerHandle_Teleport, this, &ASGideon_Ability_Teleport::TriggerTeleport, 0.2f);
}

void ASGideon_Ability_Teleport::TriggerTeleport()
{
	//Teleport to projectile location and destroy the projectile
	GetInstigator()->SetActorLocation(this->GetActorLocation());
	Destroy();
}
