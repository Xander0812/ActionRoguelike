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

	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>("EffectComp");
	EffectComp->SetupAttachment(SphereComp);

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComp");
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bInitialVelocityInLocalSpace = true;
	MovementComp->ProjectileGravityScale = 0;
}

// Called when the game starts or when spawned
void ASProjectileBaseClass::BeginPlay()
{
	Super::BeginPlay();

	//We need to ignore instagator (who spawned the projectile) to not generate hit and overlap events
	SphereComp->IgnoreActorWhenMoving(GetInstigator(), true);
}

// Called every frame
void ASProjectileBaseClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

