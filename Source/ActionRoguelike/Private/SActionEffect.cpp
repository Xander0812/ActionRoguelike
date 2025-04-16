// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionEffect.h"
#include <SAttributeComponent.h>
#include "SActionComponent.h"
#include "GameFramework/GameStateBase.h"


USActionEffect::USActionEffect()
{
	bAutoStart = true;

	TickingDuration = MaxDuration;
}

void USActionEffect::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	if(MaxDuration > 0.f)
	{
		FTimerDelegate _timerDelegateDuration;
		_timerDelegateDuration.BindUFunction(this, "TimerTick", Instigator);
		GetWorld()->GetTimerManager().SetTimer(DurationHandle, _timerDelegateDuration, 0.1, true);
	}

	if (Period > 0.f)
	{ 
		FTimerDelegate _timerDelegatePeriod;
		_timerDelegatePeriod.BindUFunction(this, "ExecutePeriodEffect", Instigator);

		GetWorld()->GetTimerManager().SetTimer(PeriodHandle, _timerDelegatePeriod, Period, true);

	}
}

void USActionEffect::StopAction_Implementation(AActor* Instigator)
{
	if(GetWorld()->GetTimerManager().GetTimerRemaining(PeriodHandle) < KINDA_SMALL_NUMBER)
	{
		ExecutePeriodEffect(Instigator);
	}

	Super::StopAction_Implementation(Instigator);

	GetWorld()->GetTimerManager().ClearTimer(DurationHandle);
	GetWorld()->GetTimerManager().ClearTimer(PeriodHandle);

	USActionComponent* _owningComp = GetOwningComponent();
	if(_owningComp)
	{
		_owningComp->RemoveAction(this);
	}
}

void USActionEffect::TimerTick(AActor* Instigator)
{
	TickingDuration -= 0.1f;

	if(TickingDuration <= 0)
	{
		StopAction(Instigator);
	}
}

void USActionEffect::AddTime()
{
	TickingDuration = MaxDuration;
}

float USActionEffect::GetTimeRemaining() const
{
	/*AGameStateBase* _gs = GetWorld()->GetGameState<AGameStateBase>();

	if(_gs)
	{
		float _endTime = TimeStarted + MaxDuration;
		return _endTime - _gs->GetServerWorldTimeSeconds();
	}
	return MaxDuration;*/
	return TickingDuration;
}

void USActionEffect::ExecutePeriodEffect_Implementation(AActor* Instigator)
{
	/*if(!ensure(Instigator))
	{
		return;
	}

	USAttributeComponent* _attributeComp = USAttributeComponent::GetAttributes(Instigator);

	if(_attributeComp)
	{
		_attributeComp->ApplyHealthChange(10, Instigator);
	}*/
}