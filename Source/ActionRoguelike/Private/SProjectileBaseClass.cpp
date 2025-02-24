// Fill out your copyright notice in the Description page of Project Settings.

#include "SProjectileBaseClass.h"
	
// Sets default values
ASProjectileBaseClass::ASProjectileBaseClass()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetCollisionProfileName("Projectile");
	RootComponent = SphereComp;

	BaseParticleEffect = CreateDefaultSubobject<UParticleSystemComponent>("EffectComp");
	BaseParticleEffect->SetupAttachment(SphereComp);

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComp");
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bInitialVelocityInLocalSpace = true;
	MovementComp->ProjectileGravityScale = 0;

	SetReplicates(true);
}

void ASProjectileBaseClass::OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSoundCue, GetActorLocation());
		//We spawn Emmiter to play animation of teleportation
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExtraParticleEffect, GetActorLocation(), GetActorRotation(), true);
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), CameraShakeEffect, GetActorLocation(), /*CameraShakeInnerRadius*/ 100, /*CameraShakeOuterRadius*/ 500);
}

// Called when the game starts or when spawned
void ASProjectileBaseClass::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::SpawnSoundAttached(LoopSoundCue, this->GetRootComponent(), TEXT("SphereComp"), GetActorLocation(), EAttachLocation::KeepRelativeOffset, true);
	//We need to ignore instagator (who spawned the projectile) to not generate hit and overlap events
	SphereComp->IgnoreActorWhenMoving(GetInstigator(), true);
}

// Called every frame
void ASProjectileBaseClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

