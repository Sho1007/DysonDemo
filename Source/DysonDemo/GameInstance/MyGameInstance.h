// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/DataTable.h"

#include "LevelSequence.h"

#include "MyGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FProcessData : public FTableRowBase 
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	int32 Process;
	UPROPERTY(EditDefaultsOnly)
	int32 Num;
	UPROPERTY(EditDefaultsOnly)
	int32 Cam;

	UPROPERTY(EditDefaultsOnly)
	FText ProcessText;
	UPROPERTY(EditDefaultsOnly)
	FText NumText;
	UPROPERTY(EditDefaultsOnly)
	FText CamText;
};
UCLASS()
class DYSONDEMO_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
private:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (AllowPrivateAccess = true))
	bool bIsAssembleMode = false;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Meta = (AllowPrivateAccess = true))
	TArray<ULevelSequence*> SequenceArray;
};