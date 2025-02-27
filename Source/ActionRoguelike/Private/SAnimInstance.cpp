// Fill out your copyright notice in the Description page of Project Settings.


#include "SAnimInstance.h"
#include "GameplayTagContainer.h"

void USAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	AActor* _owningActor = GetOwningActor();

	if(_owningActor)
	{
		ActionComp = Cast<USActionComponent>(_owningActor->GetComponentByClass(USActionComponent::StaticClass()));
	}
}

void USAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	static FGameplayTag _stunnedTag = FGameplayTag::RequestGameplayTag("Status.Stunned");

	if(ActionComp)
	{
		bIsStunned = ActionComp->ActiveGameplayTags.HasTag(_stunnedTag);
	}
}
