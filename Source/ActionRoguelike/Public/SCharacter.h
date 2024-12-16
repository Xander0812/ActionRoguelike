// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
class USInteractionComponent;
class UAnimMontage;
struct FInputActionValue;

UCLASS()
class ACTIONROGUELIKE_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> BlackHoleClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> TeleportClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* AttackAnim;

	/* timers to keep track of animations */
	FTimerHandle TimerHandle_PrimaryAttack;

	FTimerHandle TimerHandle_BlackHole;

	FTimerHandle TimerHandle_Teleport;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere)
	USInteractionComponent* InteractionComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/* Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/* Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/* Primary Attack Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PrimaryAttackAction;

	/* BlackHole Ability Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* BlackHoleAbilityAction;

	/* Teleport Ability Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* TeleportAbilityAction;

	/* Primary Interaction Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PrimaryInteractAction;


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* Called for movement input */
	void Move(const FInputActionValue& Value);

	/* Called for looking input */
	void Look(const FInputActionValue& Value);

	/* Actions that are called on action to play animation and start the timer */
	void PrimaryAttack();

	void BlackHoleAbility();

	void TeleportAbility();

	/* Actions that are called after timer stops */
	void PrimaryAttack_TimeElapsed();

	void BlackHoleAbility_TimeElapsed();

	void TeleportAbility_TimeElapsed();

	/* Interact Action */
	void PrimaryInteract();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
