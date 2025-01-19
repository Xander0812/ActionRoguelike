// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/SBTService_IsBotOnLowHealth.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SAttributeComponent.h"



void USBTService_IsBotOnLowHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	//We simply check if bot is on low health

	UBlackboardComponent* _blackboardComp = OwnerComp.GetBlackboardComponent();
	if (ensure(_blackboardComp))
	{
		AAIController* _controller = OwnerComp.GetAIOwner();
		if (ensure(_controller))
		{
			APawn* _AIPawn = _controller->GetPawn();
			if (ensure(_AIPawn))
			{
				USAttributeComponent* _attributeComp = USAttributeComponent::GetAttributes(_AIPawn);
				if (_attributeComp)
				{
					_blackboardComp->SetValueAsBool(OnLowHealthKey.SelectedKeyName, _attributeComp->GetHealthPercent() <= LowHealthPercent);
				}
			}
		}
	}
}
