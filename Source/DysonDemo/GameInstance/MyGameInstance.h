// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include <MovieScene/Public/MovieSceneSequencePlaybackSettings.h>

#include "MyGameInstance.generated.h"

class ULevelSequence;
class ALevelSequenceActor;
class ADemoGameMode;
UCLASS()
class DYSONDEMO_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	ULevelSequence* GetLevelSequence(int32 SequenceIndex) const;

	void InitLevelSequence(ADemoGameMode* NewGameMode);
	void ResetSequence(int32 Index);
	void SetSequence(int32 Index, float PlayRate);
	void SetPosition(int32 Index, bool bIsAssembleMode);
	void PlaySequence(int32 Index, bool bIsAssembleMode);
	void SetPlayRate(int32 Index, float NewPlayRate);
	void Pause(int32 Index);
	UFUNCTION()
	void OnSequenceFinished();
private:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Meta = (AllowPrivateAccess = true))
	TArray<ULevelSequence*> SequenceArray;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Meta = (AllowPrivateAccess = true))
	TArray<ALevelSequenceActor*> SequenceActorArray;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Meta = (AllowPrivateAccess = true))
	ADemoGameMode* GameMode;
};