// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SActionComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActionStateChanged, USActionComponent*, OwningComp, USAction*, Action);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API USActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tags")
	FGameplayTagContainer ActiveGameplayTags;

	UFUNCTION(BlueprintCallable, Category = " Actions")
	void AddAction(AActor* Instigator ,TSubclassOf<USAction> ActionClass);

	UFUNCTION(BlueprintCallable, Category = " Actions")
	void RemoveAction(USAction* ActionToRemove);

	UPROPERTY(BlueprintAssignable)
	FOnActionStateChanged OnActionStarted;

	UPROPERTY(BlueprintAssignable)
	FOnActionStateChanged OnActionStopped;

	UFUNCTION(BlueprintCallable, Category = " Actions")
	bool StartActionByName(AActor* Instigator, FName ActionName);

	UFUNCTION(BlueprintCallable, Category = " Actions")
	bool StopActionByName(AActor* Instigator, FName ActionName);

	USActionComponent();

	UFUNCTION(BlueprintCallable, Category = "Actions")
	static USActionComponent* GetActionsComponent(AActor* FromActor);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	bool HasAction(TSubclassOf<USAction> InputAction);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	USAction* GetAction(TSubclassOf<USAction> InputAction);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	USAction* GetActionByName(FName ActionName);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	bool HasActionByName(FName ActionName);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	static USActionComponent* GetActionComponent(AActor* FromActor);

protected:

	UFUNCTION(Server, Reliable)
	void ServerStartAction(AActor* Instigator, FName ActionName);

	UFUNCTION(Server, Reliable)
	void ServerStopAction(AActor* Instigator, FName ActionName);

	/*Granted Abilities at game start*/
	UPROPERTY(EditAnywhere, Category = "Actions")
	TArray<TSubclassOf<USAction>> DefaultActions;

	UPROPERTY(BlueprintReadOnly, Replicated)
	TArray<USAction*> Actions;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
};
