// Fill out your copyright notice in the Description page of Project Settings.


#include "SSaveGameSubsystem.h"
#include "EngineUtils.h"
#include "SGameplayInterface.h"
#include "SPlayerState.h"
#include "SSaveGame.h"
#include "SSaveGameSettings.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include <Logging/StructuredLog.h>

void USSaveGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const USSaveGameSettings* _sgSettings = GetDefault<USSaveGameSettings>();

	CurrentSlotName = _sgSettings->SaveSlotName;

	UDataTable* _dummyTable = _sgSettings->DummyTablePath.LoadSynchronous();
}

void USSaveGameSubsystem::HandleStartingNewPlayer(AController* NewPlayer)
{
	ASPlayerState* _ps = NewPlayer->GetPlayerState<ASPlayerState>();
	if(ensure(_ps))
	{
		_ps->LoadPlayerState(CurrentSaveGame);
	}
}

bool USSaveGameSubsystem::OverrideSpawnTransform(AController* NewPlayer)
{
	if(!IsValid(NewPlayer))
	{
		return false;
	}

	APlayerState* _ps = NewPlayer->GetPlayerState<APlayerState>();
	if(_ps == nullptr)
	{
		return false;
	}

	if(APawn* _myPawn = _ps->GetPawn())
	{
		FPlayerSaveData* _foundData = CurrentSaveGame->GetPlayerData(_ps);
		if(_foundData && _foundData->bResumeAtTransform)
		{
			_myPawn->SetActorLocation(_foundData->Location);
			_myPawn->SetActorRotation(_foundData->Rotation);

			// PlayerState onwer is a (Player)Conroller
			AController* _pc = CastChecked<AController>(_ps->GetOwner());
			// Set control rotation to change camera direction, setting Pawn rotation is not enough
			_pc->SetControlRotation(_foundData->Rotation);

			return true;
		}
	}

	return false;
}

void USSaveGameSubsystem::SetSlotName(FString NewSlotName)
{
	if(NewSlotName.Len() == 0)
	{
		return;
	}

	CurrentSlotName = NewSlotName;
}

void USSaveGameSubsystem::WriteSaveGame()
{
	// Clear arrays, may contain data from previosly loaded SaveGame
	CurrentSaveGame->SavedActors.Empty();
	CurrentSaveGame->SavedPlayers.Empty();

	AGameStateBase* _gs = GetWorld()->GetGameState();
	if(_gs == nullptr)
	{
		UE_LOGFMT(LogTemp, Warning, "No game state!");
		return;
	}

	// Iterate all player states, we don't have proper ID to match yet (requiers Steam or EQS)
	for (int32 i = 0; i < _gs->PlayerArray.Num(); i++)
	{
		ASPlayerState* _ps = Cast<ASPlayerState>(_gs->PlayerArray[i]);
		if (_ps)
		{
			_ps->SavePlayerState(CurrentSaveGame);
			break; //single player only at this point
		}
	}


	//Itirate the entire world of actors
	for (FActorIterator _it(GetWorld()); _it; ++_it)
	{
		AActor* _actor = *_it;
		//only interested in our 'gameplay actors'
		if (!_actor->Implements<USGameplayInterface>())
		{
			continue;
		}

		FActorSaveData _actorData;
		_actorData.ActorName = _actor->GetName();
		_actorData.Transform = _actor->GetActorTransform();

		//Pass in the array to fill with data from Actor
		FMemoryWriter _memWriter(_actorData.ByteData);
		FObjectAndNameAsStringProxyArchive _ar(_memWriter, true);

		// Find ONLY viriables with UPROPERTY(SaveGame)
		_ar.ArIsSaveGame = true;

		//Converts Actpor's SaveGame UPROPERTIES to binary array
		_actor->Serialize(_ar);

		CurrentSaveGame->SavedActors.Add(_actorData);
	}

	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, CurrentSlotName, 0);

	OnSaveGameWritten.Broadcast(CurrentSaveGame);
}

void USSaveGameSubsystem::LoadSaveGame(FString InSlotName /* = "" */)
{
	//Update slot name first IF specified, otherwise keeps default one
	SetSlotName(InSlotName);

	if (UGameplayStatics::DoesSaveGameExist(CurrentSlotName, 0))
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::LoadGameFromSlot(CurrentSlotName, 0));
		if (CurrentSaveGame == nullptr)
		{
			UE_LOGFMT(LogTemp, Warning, "Failed to load SaveGame data.");
			return;
		}

		UE_LOGFMT(LogTemp, Warning, "loaded SaveGame data.");

		for (FActorIterator _it(GetWorld()); _it; ++_it)
		{
			AActor* _actor = *_it;
			//only interested in our 'gameplay actors'
			if (!_actor->Implements<USGameplayInterface>())
			{
				continue;
			}

			for (FActorSaveData _actorData : CurrentSaveGame->SavedActors)
			{
				if (_actorData.ActorName == _actor->GetName())
				{
					_actor->SetActorTransform(_actorData.Transform);

					//Get filled array from Actor SaveGame
					FMemoryReader _memReader(_actorData.ByteData);
					FObjectAndNameAsStringProxyArchive _ar(_memReader, true);
					_ar.ArIsSaveGame = true;

					//Converts binary array back to actor's variables
					_actor->Serialize(_ar);

					ISGameplayInterface::Execute_OnActorLoaded(_actor);

					break;
				}
			}
		}
		OnSaveGameLoaded.Broadcast(CurrentSaveGame);
	}
	else
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::CreateSaveGameObject(USSaveGame::StaticClass()));

		UE_LOGFMT(LogTemp, Warning, "Created new SaveGame data. ");
	}


}
