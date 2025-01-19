// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/SBTTask_HealSelf.h"
#include "Engine/World.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SAttributeComponent.h"
#include "TimerManager.h"


EBTNodeResult::Type USBTTask_HealSelf::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* _blackboardComp = OwnerComp.GetBlackboardComponent();
	if (!ensure(_blackboardComp))
	{
		return EBTNodeResult::Failed;
	}
	
	AAIController* _controller = OwnerComp.GetAIOwner();
	if (!ensure(_controller))
	{
		return EBTNodeResult::Failed;
	}
	
	APawn* _AIPawn = _controller->GetPawn();
	if (!ensure(_AIPawn))
	{
		return EBTNodeResult::Failed;
	}
	
	USAttributeComponent* _attributeComp = USAttributeComponent::GetAttributes(_AIPawn);
	if (!_attributeComp)
	{
		return EBTNodeResult::Failed;
	}

	if (_blackboardComp->GetValueAsBool(OnLowHealthKey.SelectedKeyName) && _blackboardComp->GetValueAsBool(CanHealSelf.SelectedKeyName))
	{
		//Start task to hide and heal self when low HP

		_attributeComp->ApplyHealthChange(HealAmount, _AIPawn);
		_blackboardComp->SetValueAsBool(OnLowHealthKey.SelectedKeyName, false);
		_blackboardComp->SetValueAsBool(CanHealSelf.SelectedKeyName, false);

		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}

USBTTask_HealSelf::USBTTask_HealSelf()
{
	//bCreateNodeInstance = true;
}

