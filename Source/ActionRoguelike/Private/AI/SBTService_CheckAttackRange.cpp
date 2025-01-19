// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckAttackRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"


void USBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* _blackboardComp = OwnerComp.GetBlackboardComponent();
	if(ensure(_blackboardComp))
	{
		AActor* _targetActor = Cast<AActor>(_blackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));

		if(_targetActor)
		{
			AAIController* _controller = OwnerComp.GetAIOwner();
			if(ensure(_controller))
			{
				APawn* _AIPawn = _controller->GetPawn();
				if(ensure(_AIPawn))
				{
					// Check distance between AI pawn and Target actor

					float _distance = FVector::Distance(_targetActor->GetActorLocation(), _AIPawn->GetActorLocation());

					bool _bWithinRange = _distance < AttackRange;

					bool _bHasLineOfSight = false;

					if(_bWithinRange)
					{
						_bHasLineOfSight = _controller->LineOfSightTo(_targetActor);
					}

					_blackboardComp->SetValueAsBool(AttackRangeKey.SelectedKeyName, _bWithinRange && _bHasLineOfSight);
				}
			}
		}
	}
}
