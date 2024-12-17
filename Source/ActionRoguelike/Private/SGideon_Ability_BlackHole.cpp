// Fill out your copyright notice in the Description page of Project Settings.


#include "SGideon_Ability_BlackHole.h"

ASGideon_Ability_BlackHole::ASGideon_Ability_BlackHole()
{
	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));

	MovementComp->InitialSpeed = 1200;

	RadialForceComp->SetupAttachment(SphereComp);
	RadialForceComp->ForceStrength = -2000;
	RadialForceComp->bAutoActivate = false;
	RadialForceComp->Radius = 700;

	//BaseMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMeshComp"));
	//BaseMeshComp->SetupAttachment(SphereComp);

	//RimMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RimMeshComp"));
	//RimMeshComp->SetupAttachment(BaseMeshComp);

	//VertexMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VertexMeshComp"));
	//VertexMeshComp->SetupAttachment(BaseMeshComp);

	//Set OnComponentHit action event
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASGideon_Ability_BlackHole::OnComponentOverlap);
}

void ASGideon_Ability_BlackHole::BeginPlay()
{
	Super::BeginPlay();

	//Start animation timer. We play particle effect after timer runs out
	GetWorldTimerManager().SetTimer(TimerHandle_FlyTime, this, &ASGideon_Ability_BlackHole::DestroyOnAnimationEnd, 5.0f);
}

void ASGideon_Ability_BlackHole::OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnComponentOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	OtherActor->Destroy();
}

void ASGideon_Ability_BlackHole::DestroyOnAnimationEnd()
{
	Destroy();
}