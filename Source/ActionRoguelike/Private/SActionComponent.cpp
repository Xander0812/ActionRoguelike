// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionComponent.h"
#include "SAction.h"
#include "../ActionRoguelike.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "SActionEffect.h"


DECLARE_CYCLE_STAT(TEXT("StartActionByName"), STAT_StartActionByName, STATGROUP_STANFORD);

USActionComponent::USActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}


void USActionComponent::BeginPlay()
{
	Super::BeginPlay();

	//Server only
	if (GetOwner()->HasAuthority())
	{
		for (TSubclassOf<USAction> _actionClass : DefaultActions)
		{
			AddAction(GetOwner(), _actionClass);
		}
	}
}

void USActionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//Stop ALL actions
	TArray<USAction*> _actionsCopy = Actions;
	for(USAction* _action : _actionsCopy)
	{
		if (_action && _action->IsActionRunning())
		{
			_action->StopAction(GetOwner());
		}
	}

	Super::EndPlay(EndPlayReason);
}


// Called every frame
void USActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//FString _debugMsg = GetNameSafe(GetOwner()) + " : " + ActiveGameplayTags.ToStringSimple();
	//GEngine->AddOnScreenDebugMessage(-1, 0, FColor::White, _debugMsg);

	for(USAction* _action : Actions)
	{
		FColor _textColor = _action->IsActionRunning() ? FColor::Blue : FColor::White;
		FString _actionMsg = FString::Printf(TEXT("[%s] Action: %s"), *GetNameSafe(GetOwner()), *GetNameSafe(_action));

		LogOnScreen(this, _actionMsg, _textColor, 0.0f);
	}
}

void USActionComponent::AddAction(AActor* Instigator,TSubclassOf<USAction> ActionClass)
{
	if(!ensure(ActionClass))
	{
		return;
	}

	if(!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Client attempting to AddAction. [Class: %s]"), *GetNameSafe(ActionClass));
		return;
	}

	if(HasAction(ActionClass))
	{
		if(ActionClass->IsChildOf(USActionEffect::StaticClass()))
		{
			USActionEffect* _ae = Cast<USActionEffect>(GetAction(ActionClass));

			if (_ae) 
			{
				_ae->AddTime();
				return;
			}
		}
	}

	USAction* _newAction = NewObject<USAction>(GetOwner(), ActionClass);
	if(ensure(_newAction))
	{
		//prevent from adding new actions
		
		_newAction->Initialize(this);

		Actions.Add(_newAction);

		if(_newAction-> bAutoStart && ensure(_newAction->CanStart(Instigator)))
		{
			_newAction->StartAction(Instigator);
		}
	}
}

void USActionComponent::RemoveAction(USAction* ActionToRemove)
{
	if (!ensure(ActionToRemove && !ActionToRemove->IsActionRunning()))
	{
		return;
	}

	Actions.Remove(ActionToRemove);
}

bool USActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
	SCOPE_CYCLE_COUNTER(STAT_StartActionByName);

	for(USAction* _action:Actions)
	{
		if(_action && _action->ActionName == ActionName)
		{
			if(!_action->CanStart(Instigator))
			{
				FString _failedMsg = FString::Printf(TEXT("Failed to run: %s"), *ActionName.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, _failedMsg);
				continue;
			}

		    //Is client
			if(!GetOwner()->HasAuthority())
				ServerStartAction(Instigator, ActionName);

			TRACE_BOOKMARK(TEXT("StartAction::%s"), *GetNameSafe(_action))

			_action->StartAction(Instigator);
			return true;
		}
	}

	return false;
}

bool USActionComponent::StopActionByName(AActor* Instigator, FName ActionName)
{
	for (USAction* _action : Actions)
	{
		if (_action && _action->ActionName == ActionName)
		{
			if(_action->IsActionRunning())
			{
				//Is client
				if (!GetOwner()->HasAuthority())
					ServerStopAction(Instigator, ActionName);

				_action->StopAction(Instigator);
				return true;
			}
		}
	}

	return false;
}

bool USActionComponent::HasAction(TSubclassOf<USAction> InputAction)
{
	for (USAction* _action : Actions)
	{
		if (_action && _action->IsA(InputAction))
		{
			return true;
		}
	}

	return false;
}

bool USActionComponent::HasActionByName(FName ActionName)
{
	for (USAction* _action : Actions)
	{
		if (_action && _action->ActionName == ActionName)
		{
			return true;
		}
	}

	return false;
}

USAction* USActionComponent::GetAction(TSubclassOf<USAction> InputAction)
{
	for (USAction* _action : Actions)
	{
		if (_action && _action->IsA(InputAction))
		{
			return _action;
		}
	}
	return nullptr;
}

USAction* USActionComponent::GetActionByName(FName ActionName)
{
	for (USAction* _action : Actions)
	{
		if (_action && _action->ActionName == ActionName)
		{
			return _action;
		}
	}

	return nullptr;
}

USActionComponent* USActionComponent::GetActionsComponent(AActor* FromActor)
{
	if (FromActor)
	{
		return Cast<USActionComponent>(FromActor->GetComponentByClass(USActionComponent::StaticClass()));
	}

	return nullptr;
}

USActionComponent* USActionComponent::GetActionComponent(AActor* FromActor)
{
	if (FromActor)
	{
		return Cast<USActionComponent>(FromActor->GetComponentByClass(USActionComponent::StaticClass()));
	}

	return nullptr;
}

void USActionComponent::ServerStartAction_Implementation(AActor* Instigator, FName ActionName)
{
	StartActionByName(Instigator, ActionName);
}

void USActionComponent::ServerStopAction_Implementation(AActor* Instigator, FName ActionName)
{
	StopActionByName(Instigator, ActionName);
}

bool USActionComponent::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool _wroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for(USAction* _action : Actions)
	{
		if(_action)
		{
			_wroteSomething |= Channel->ReplicateSubobject(_action, *Bunch, *RepFlags);
		}
	}

	return _wroteSomething;
}

void USActionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USActionComponent, Actions);
}
