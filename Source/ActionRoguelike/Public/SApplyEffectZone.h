// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "SActionEffect.h"
#include "SApplyEffectZone.generated.h"


UCLASS()
class ACTIONROGUELIKE_API ASApplyEffectZone : public AActor
{
	GENERATED_BODY()
	
public:	
	ASApplyEffectZone();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* BoxComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UDecalComponent* DecalComp;

	UPROPERTY(EditDefaultsOnly, Category = "ActionEffect")
	TSubclassOf<USActionEffect> EffectClassToApply;

	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UText3DComponent* Text3D;*/

protected:

	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void BeginPlay() override;

};
