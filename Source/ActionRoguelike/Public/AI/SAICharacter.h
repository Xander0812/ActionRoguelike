// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SAttributeComponent.h"
#include "SWorldUserWidget.h"
#include <SActionComponent.h>
#include "SAICharacter.generated.h"

class UPawnSensingComponent;
class USAttributeComponent;

UCLASS()
class ACTIONROGUELIKE_API ASAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASAICharacter();

protected:

	void SetTargetActor(AActor* NewActor);

	USWorldUserWidget* ActiveHealthBar;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HealthBarWidgetClass;

	/*Name for material function to call on hit*/
	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName TimeToHitParamName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USAttributeComponent* AttributeComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USActionComponent* ActionComp;

	/*Pawn sensing reference to spot the player*/
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPawnSensingComponent* PawnSensingComp;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<USWorldUserWidget> TargetSpotedWidgetClass;

	UPROPERTY()
	USWorldUserWidget* TargetSpotedWidgetInstance;

	UFUNCTION()
	void OnHealthChanged(AActor* InstagatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayerSpotted(APawn* Target);
public:	

	virtual void PostInitializeComponents() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);

	UFUNCTION(BlueprintCallable)
	AActor* GetTargetActor() const;
};
