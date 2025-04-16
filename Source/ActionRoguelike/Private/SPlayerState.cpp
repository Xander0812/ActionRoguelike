// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"

#include "Net/UnrealNetwork.h"
#include "SSaveGame.h"

ASPlayerState::ASPlayerState()
{
	//SetIsReplicatedByDefault(true);
	SetReplicates(true);
}

bool ASPlayerState::AddCredits(int32 Delta)
{
	if(!ensure(Delta > 0.0f))
	{
		return false;
	}
	CreditAmount += Delta;

	OnRep_Credits(CreditAmount - Delta);

	return true;
}

bool ASPlayerState::RemoveCredits(int32 Delta)
{
	if (!ensure(Delta > 0.0f))
	{
		return false;
	}

	if(CreditAmount < Delta)
	{
		return false;
	}
	
	CreditAmount -= Delta;

	OnRep_Credits(CreditAmount + Delta);

	return true;
}

bool ASPlayerState::UpdatePersonalRecord(float NewTime)
{
	// Higher time is better
	if (NewTime > PersonalRecordTime)
	{
		float OldRecord = PersonalRecordTime;

		PersonalRecordTime = NewTime;

		OnRecordTimeChanged.Broadcast(this, PersonalRecordTime, OldRecord);

		return true;
	}

	return false;
}

void ASPlayerState::SavePlayerState_Implementation(USSaveGame* SaveObject)
{
	if (SaveObject)
	{
		// Gather all relevant data for player
		FPlayerSaveData SaveData;
		SaveData.Credits = CreditAmount;
		SaveData.PersonalRecordTime = PersonalRecordTime;
		// Stored as FString for simplicity (original Steam ID is uint64)
		SaveData.PlayerID = GetUniqueId()->ToString();

		// May not be alive while we save
		if (APawn* MyPawn = GetPawn())
		{
			SaveData.Location = MyPawn->GetActorLocation();
			SaveData.Rotation = MyPawn->GetActorRotation();
			SaveData.bResumeAtTransform = true;
		}

		SaveObject->SavedPlayers.Add(SaveData);
	}
}

void ASPlayerState::LoadPlayerState_Implementation(USSaveGame* SaveObject)
{
	if (SaveObject)
	{
		FPlayerSaveData* FoundData = SaveObject->GetPlayerData(this);
		if (FoundData)
		{
			// Makes sure we trigger credits changed event
			AddCredits(FoundData->Credits);

			PersonalRecordTime = FoundData->PersonalRecordTime;
		}
	}
}

int32 ASPlayerState::GetCurrentCredits() const
{
	return CreditAmount;
}

void ASPlayerState::OnRep_Credits(int32 OldCredits)
{
	OnCreditsChanged.Broadcast(this, CreditAmount, CreditAmount - OldCredits);
}

void ASPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerState, CreditAmount);
}