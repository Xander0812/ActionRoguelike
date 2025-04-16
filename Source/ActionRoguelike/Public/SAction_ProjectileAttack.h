// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAction.h"
#include "SProjectileBaseClass.h"

#include "SAction_ProjectileAttack.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USAction_ProjectileAttack : public USAction
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(VisibleAnywhere, Category = "Attack")
	FName HandSocketName;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<ASProjectileBaseClass> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* AttackAnim;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackAnimDelay;

	UPROPERTY(EditAnywhere, Category = "Attack")
	int ProjectileArraySize;

	UPROPERTY(VisibleAnywhere, Category = "Attack")
	TArray<ASProjectileBaseClass*> ProjectileArray;

	UFUNCTION()
	void AttackDelay_Elapsed(ACharacter* InstigatorCharacter);

	UFUNCTION()
	FTransform GetProjectileDirection(ACharacter* InstagitorCharacter);

	UFUNCTION()
	ASProjectileBaseClass* GetFreeProjectile();

public:

	void Initialize(USActionComponent* NewActionComp) override;

	virtual void StartAction_Implementation(AActor* Instigator) override;

	USAction_ProjectileAttack();
};
