// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include <SSaveGame.h>
#include "SPlayerState.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCreditsChanged, ASPlayerState*, PlayerState, int32, NewCredits, int32, Delta);

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Credits")
	int32 CreditAmount;

	ASPlayerState();
public:
	UFUNCTION(BlueprintCallable, Category = "Credits")
	bool AddCredits(int32 Delta, AActor* InstigatorActor);

	UFUNCTION(BlueprintCallable, Category = "Credits")
	bool RemoveCredits(int32 Delta, AActor* InstigatorActor);

	UFUNCTION(BlueprintCallable, Category = "Credits")
	int32 GetCurrentCredits() const;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCreditsChanged OnCreditsChanged;

	UFUNCTION(NetMulticast, Reliable) 
	void MulticastCreditsChanged(AActor* InstigatorActor, float NewCreditAmount, float Delta);

	UFUNCTION(BlueprintNativeEvent)
	void SavePlayerState(USSaveGame* SaveObject);

	UFUNCTION(BlueprintNativeEvent)
	void LoadPlayerState(USSaveGame* SaveObject);
};
