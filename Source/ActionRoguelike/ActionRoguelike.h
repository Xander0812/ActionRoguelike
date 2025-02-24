// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"



static void LogOnScreen(UObject* WorldContext, FString Msg, FColor Color = FColor::White, float Duration = 5.0f)
{
	if(!ensure(WorldContext))
	{
		return;
	}

	UWorld* _world = WorldContext ->GetWorld();

	if(!ensure(_world))
	{
		return;
	}

	FString _netPrefix = _world->IsNetMode(NM_Client) ? "[CLIENT] " : "[SERVER] ";

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, _netPrefix + Msg);
	}
}