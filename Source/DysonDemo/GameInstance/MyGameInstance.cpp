// Fill out your copyright notice in the Description page of Project Settings.

#include "../GameInstance/MyGameInstance.h"

#include <LevelSequence.h>

ULevelSequence* UMyGameInstance::GetLevelSequence(int32 SequenceIndex) const
{
	if (SequenceIndex < 0 || SequenceArray.Num() <= SequenceIndex) return nullptr;

	return SequenceArray[SequenceIndex];
}