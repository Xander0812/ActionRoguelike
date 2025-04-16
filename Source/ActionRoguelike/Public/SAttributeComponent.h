// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SAttributeComponent.generated.h"



//DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, float, NewHealth, float, Delta);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnRageChanged, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, float, NewRage, float, Delta);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnAttributeChanged, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, float, NewValue, float, Delta);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API USAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	static USAttributeComponent* GetAttributes(AActor* FromActor);

	UFUNCTION(BlueprintCallable, Category = "Attributes", Meta = (DisplayName = "IsAlive"))
	static bool IsActorAlive(AActor* Actor);

	// Sets default values for this component's properties
	USAttributeComponent();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Attributes")
	float CurrentHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Attributes")
	float MaxHealth;

	/*Rage*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Attributes")
	float CurrentRage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Attributes")
	float MaxRage;
	
	/*Movement Speed*/

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Attributes")
	float MovementSpeedBuffBonus;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Attributes")
	float SprintSpeedBonus;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Attributes")
	float CurrentMovementSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Attributes")
	float DefaultMovementSpeed;



	UFUNCTION(NetMulticast, Reliable) //@FIXME: mark as unreliable once we moved the 'state' out of SCharacter
	void MulticastHealthChanged(AActor* InstigatorActor, float NewHealth, float Delta);

	UFUNCTION(NetMulticast, Unreliable) //@FIXME: mark as unreliable once we moved the 'state' out of SCharacter
	void MulticastRageChanged(AActor* InstigatorActor, float NewRage, float Delta);

	UFUNCTION(NetMulticast, Unreliable) //@FIXME: mark as unreliable once we moved the 'state' out of SCharacter
	void MulticastMovementSpeedChanged(AActor* InstigatorActor, float NewSpeed, float Delta);
	// HealthMax, Stamina, Strength

public:	

	UFUNCTION(BlueprintCallable)
	bool Kill(AActor* InstigatorActor);

	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FOnAttributeChanged OnHealthChanged;

	UPROPERTY(BlueprintReadWrite, Category = "VFX")
	UMaterialInstanceDynamic* DynamicOverlayMaterial;

	/* Health */

	/*Apply healing or damage*/
	UFUNCTION(BlueprintCallable)
	bool ApplyHealthChange(float Delta, AActor* InstigatorActor);

	/*Get Object with attribute comp MAXIMUM health*/
	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const;

	/*Get Object with attribute comp CURRENT health*/
	UFUNCTION(BlueprintCallable)
	float GetCurrentHealth() const;

	/*Is called Object on full health?*/
	UFUNCTION(BlueprintCallable)
	bool IsFullHealth() const;

	/*Is called Object Alive?*/
	UFUNCTION(BlueprintCallable)
	bool IsAlive() const;

	/*Get current health percent*/
	UFUNCTION(BlueprintCallable)
	float GetHealthPercent() const;

	/* Rage */

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnRageChanged;

	/*Add Rage or use it to cast ability*/
	UFUNCTION(BlueprintCallable)
	bool ApplyRageChange(float Delta, AActor* InstigatorActor);

	/*Get Object with attribute comp MAXIMUM Rage*/
	UFUNCTION(BlueprintCallable)
	float GetMaxRage() const;

	/*Get Object with attribute comp CURRENT Rage*/
	UFUNCTION(BlueprintCallable)
	float GetCurrentRage() const;

	/*Is called Object on Max Rage?*/
	UFUNCTION(BlueprintCallable)
	bool IsFullRage() const;

	/* Movement Speed */

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnSpeedChanged;

	UFUNCTION(BlueprintCallable)
	void ApplySpeedChange(AActor* InstigatorActor, float Delta);

	UFUNCTION(BlueprintCallable)
	bool ActivateSpeedBoost(float SpeedModifier);

	UFUNCTION(BlueprintCallable)
	bool DeactivateSpeedBoost();

	UFUNCTION(BlueprintCallable)
	bool ActivateSprint(float SpeedModifier);

	UFUNCTION(BlueprintCallable)
	bool DeactivateSprint();


};
