// Fill out your copyright notice in the Description page of Project Settings.


#include "SExploadingBarrel.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"

//#include UE_INLINE_GENERATED_CPP_BY_NAME(SExplosiveBarrel)

// Sets default values
ASExploadingBarrel::ASExploadingBarrel()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	RootComponent = MeshComp;


	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	RadialForceComp->SetupAttachment(MeshComp);
	RadialForceComp->ImpulseStrength = 2000;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->bAutoActivate = false;
	RadialForceComp->Radius = 700;

}

void ASExploadingBarrel::Explode()
{
	RadialForceComp->FireImpulse();
}

//float ASExploadingBarrel::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
//	AActor* DamageCauser)
//{
//	// Could safely skip the base logic...
//	//DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
//
//	RadialForceComp->FireImpulse();
//
//	//ExplosionComp->Activate();
//
//	// @todo: cause damage to other stuff around it
//
//	//UE_LOGFMT(LogGame, Log, "OnActorHit in Explosive Barrel");
//
//	// Warnings as structed logs even show up in the "Message Log" window of UnrealEd
//	//UE_LOGFMT(LogGame, Warning, "OnActorHit, OtherActor: {name}, at game time: {timeseconds}", GetNameSafe(DamageCauser), GetWorld()->TimeSeconds);
//
//	//FString CombinedString = FString::Printf(TEXT("Hit at location: %s"), *Hit.ImpactPoint.ToString());
//	//DrawDebugString(GetWorld(), Hit.ImpactPoint, CombinedString, nullptr, FColor::Green, 2.0f, true);
//
//	return DamageAmount;
//}

