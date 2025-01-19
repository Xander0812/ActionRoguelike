// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"

#include "SAttributeComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, float, NewHealth, float, Delta);

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

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, Category = "Attributes")
	float MaxHealth;

	// HealthMax, Stamina, Strength

public:	

	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;

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
};
