// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

class ULevelSequence;
UCLASS()
class DYSONDEMO_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	ULevelSequence* GetLevelSequence(int32 SequenceIndex) const;
private:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Meta = (AllowPrivateAccess = true))
	TArray<ULevelSequence*> SequenceArray;
};