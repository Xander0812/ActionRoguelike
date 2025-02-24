// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction.h"
#include "SActionComponent.h"
#include "../ActionRoguelike.h"
#include <Net/UnrealNetwork.h>

void USAction::Initialize(USActionComponent* NewActionComp)
{
	ActionComp = NewActionComp;
}

void USAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Running: %s"), *GetNameSafe(this));

	//LogOnScreen(this, FString::Printf(TEXT("Started: %s"), *ActionName.ToString()), FColor::Green);

	USActionComponent* _owningComp = GetOwningComponent();
	_owningComp->ActiveGameplayTags.AppendTags(GrantsTags);

	RepData.bIsActionRunning = true;
	RepData.InstigatorActor = Instigator;
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Stopped: %s"), *GetNameSafe(this));

	//LogOnScreen(this, FString::Printf(TEXT("Stopped: %s"), *ActionName.ToString()), FColor::White);

	//ensureAlways(bIsActionRunning);

	USActionComponent* _owningComp = GetOwningComponent();
	_owningComp->ActiveGameplayTags.RemoveTags(GrantsTags);

	RepData.bIsActionRunning = false;
	RepData.InstigatorActor = Instigator;
}

bool USAction::CanStart_Implementation(AActor* Instigator)
{
	if(IsActionRunning())
	{
		return false;
	}

	USActionComponent* _owningComp = GetOwningComponent();

	if(_owningComp->ActiveGameplayTags.HasAny(BlockedTags))
	{
		return false;
	}
	return true;
}

UWorld* USAction::GetWorld() const
{
	//Outer is set when creating action via NewObject<T>
	AActor* _actor = Cast<AActor>(GetOuter());
	if(_actor)
	{
		return _actor->GetWorld();
	}
	return nullptr;
}

USActionComponent* USAction::GetOwningComponent() const
{
	//AActor* _actor = Cast<AActor>(GetOuter());
	//return _actor->GetComponentByClass(USActionComponent::StaticClass());

	return ActionComp;

	//return Cast<USActionComponent>(GetOuter());
}

void USAction::OnRep_RepData()
{
	if(RepData.bIsActionRunning)
	{
		StartAction(RepData.InstigatorActor);
	}
	else
	{
		StopAction(RepData.InstigatorActor);
	}
}

bool USAction::IsActionRunning() const
{
	return RepData.bIsActionRunning;
}

void USAction::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USAction, RepData);
	DOREPLIFETIME(USAction, ActionComp);
}