// Fill out your copyright notice in the Description page of Project Settings.

#include "../GameInstance/MyGameInstance.h"

#include <LevelSequence.h>
#include <LevelSequenceActor.h>
#include <LevelSequencePlayer.h>

#include "../GameMode/DemoGameMode.h"

ULevelSequence* UMyGameInstance::GetLevelSequence(int32 SequenceIndex) const
{
	if (SequenceIndex < 0 || SequenceArray.Num() <= SequenceIndex) return nullptr;

	return SequenceArray[SequenceIndex];
}

void UMyGameInstance::InitLevelSequence(ADemoGameMode* NewGameMode)
{
	UE_LOG(LogTemp, Warning, TEXT("UMyGameInstance::InitLevelSequence : Called"));
	GameMode = NewGameMode;

	SequenceActorArray.Init(nullptr, SequenceArray.Num());

	FMovieSceneSequencePlaybackSettings Settings;
	Settings.bPauseAtEnd = true;
	for (int i = 0; i < SequenceArray.Num(); ++i)
	{
		if (SequenceArray[i] && SequenceArray[i]->IsValidLowLevelFast())
		{
			ALevelSequenceActor* LevelSequenceActor;
			ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), SequenceArray[i], Settings, LevelSequenceActor);
			LevelSequenceActor->SequencePlayer->OnFinished.AddDynamic(this, &UMyGameInstance::OnSequenceFinished);
			SequenceActorArray[i] = LevelSequenceActor;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UMyGameInstance::InitLevelSequence : %d SequenceActor is Not Created"), i);
		}
	}
}

void UMyGameInstance::ResetSequence(int32 Index)
{
	UE_LOG(LogTemp, Warning, TEXT("UMyGameInstance::ResetSequence : Called"));

	if (Index < 0 || Index >= SequenceActorArray.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("UMyGameInstance::ResetSequence : Invalid Index"));
		return;
	}
	SequenceActorArray[Index]->SequencePlayer->Pause();
	//SequenceActorArray[Index]->SequencePlayer->SetPlaybackPosition(FMovieSceneSequencePlaybackParams());
}

void UMyGameInstance::SetSequence(int32 Index, float PlayRate)
{
	UE_LOG(LogTemp, Warning, TEXT("UMyGameInstance::SetSequence : Called"));

	if (Index < 0 || Index >= SequenceActorArray.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("UMyGameInstance::SetSequence : Invalid Index"));
		return;
	}

	if (SequenceActorArray[Index] == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UMyGameInstance::SetSequence : Sequence Is Null"));
		return;
	}
	
	SequenceActorArray[Index]->SequencePlayer->SetPlayRate(PlayRate);
	
	for (int i = 0; i < SequenceActorArray.Num(); ++i)
	{
		if (SequenceActorArray[i] == nullptr) continue;
		SequenceActorArray[i]->SequencePlayer->Stop();
	}
}

void UMyGameInstance::SetPosition(int32 Index, bool bIsAssembleMode)
{
	FMovieSceneSequencePlaybackParams Params;
	bIsAssembleMode ? Params.Frame.FrameNumber = SequenceActorArray[Index]->SequencePlayer->GetEndTime().Time.FrameNumber : Params.Frame.FrameNumber = SequenceActorArray[Index]->SequencePlayer->GetStartTime().Time.FrameNumber;
	SequenceActorArray[Index]->SequencePlayer->SetPlaybackPosition(Params);
}

void UMyGameInstance::PlaySequence(int32 Index, bool bIsAssembleMode)
{
	UE_LOG(LogTemp, Warning, TEXT("UMyGameInstance::PlaySequence : Called"));

	if (Index < 0 || Index >= SequenceActorArray.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("UMyGameInstance::PlaySequence : Invalid Index"));
		return;
	}

	if (SequenceActorArray[Index] == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UMyGameInstance::PlaySequence : Sequence Is Null"));
		return;
	}

	bIsAssembleMode ? SequenceActorArray[Index]->SequencePlayer->PlayReverse() : SequenceActorArray[Index]->SequencePlayer->Play();
}

void UMyGameInstance::SetPlayRate(int32 Index, float NewPlayRate)
{
	UE_LOG(LogTemp, Warning, TEXT("UMyGameInstance::SetPlayRate : Called"));

	if (Index < 0 || Index >= SequenceActorArray.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("UMyGameInstance::SetPlayRate : Invalid Index"));
		return;
	}

	if (SequenceActorArray[Index] == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UMyGameInstance::SetPlayRate : Sequence Is Null"));
		return;
	}

	SequenceActorArray[Index]->SequencePlayer->SetPlayRate(NewPlayRate);
}

void UMyGameInstance::Pause(int32 Index)
{
	UE_LOG(LogTemp, Warning, TEXT("UMyGameInstance::Pause : Called"));

	if (Index < 0 || Index >= SequenceActorArray.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("UMyGameInstance::Pause : Invalid Index"));
		return;
	}

	if (SequenceActorArray[Index] == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UMyGameInstance::Pause : Sequence Is Null"));
		return;
	}

	SequenceActorArray[Index]->SequencePlayer->Pause();
}

void UMyGameInstance::OnSequenceFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("UMyGameInstance::OnSequenceFinished : Called"));
	GameMode->OnSequenceFinished();
}