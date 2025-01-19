// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAIController.h"
#include <Kismet/GameplayStatics.h>
#include "BehaviorTree/BlackboardComponent.h"

void ASAIController::BeginPlay()
{
	Super::BeginPlay();

	if(ensureMsgf(BehaviorTree, TEXT("Behavior Tree is nullptr! Plase assign BehaviorTree in your AI Controller.")))
	{
		RunBehaviorTree(BehaviorTree);
	}


	/*APawn* _player = UGameplayStatics::GetPlayerPawn(this, 0);

	if(_player)
	{
		GetBlackboardComponent()->SetValueAsVector("MoveToLocation", _player->GetActorLocation());

		GetBlackboardComponent()->SetValueAsObject("TargetActor", _player);
	}*/
}
