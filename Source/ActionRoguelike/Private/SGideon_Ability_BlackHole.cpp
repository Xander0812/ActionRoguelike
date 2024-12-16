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

	BaseMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMeshComp"));
	BaseMeshComp->SetupAttachment(SphereComp);

	RimMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RimMeshComp"));
	RimMeshComp->SetupAttachment(BaseMeshComp);

	VertexMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VertexMeshComp"));
	VertexMeshComp->SetupAttachment(BaseMeshComp);
}