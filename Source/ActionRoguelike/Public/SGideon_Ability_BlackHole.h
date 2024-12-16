// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SProjectileBaseClass.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "SGideon_Ability_BlackHole.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASGideon_Ability_BlackHole : public ASProjectileBaseClass
{
	GENERATED_BODY()
	

protected:
	ASGideon_Ability_BlackHole();

public:
	
	UPROPERTY(VisibleAnywhere)
	URadialForceComponent* RadialForceComp;

	/* We need three meshes because dowloaded effects consists of three meshes*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* BaseMeshComp;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* RimMeshComp;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* VertexMeshComp;
};
