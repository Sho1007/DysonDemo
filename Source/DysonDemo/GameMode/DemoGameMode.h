// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "HTTP.h"

#include "DemoGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_Bool, bool, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_Float, float, NewValue);
/**
 * 
 */

class UTexture2D;
class UStaticMeshComponent;
class APartsActor;
class ALevelSequenceActor;

USTRUCT(BlueprintType)
struct FCameraSetting
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float TargetArmLength;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator SrpingArmRotation;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector SrpingArmLocation;
};

USTRUCT(BlueprintType)
struct FProcessData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FText ProcessText;
	UPROPERTY(EditAnywhere)
	TArray<APartsActor*> PartsActorArray;
	UPROPERTY(EditAnywhere)
	int32 SequenceIndex;
	UPROPERTY(EditAnywhere)
	int32 CameraIndex;
	UPROPERTY(EditAnywhere)
	TArray<FText> AssembleInstruction;
	UPROPERTY(EditAnywhere)
	TArray<FText> DisassembleInstruction;
	UPROPERTY(EditAnywhere)
	TArray<UTexture2D*> DetailImageArray;
};

UCLASS()
class DYSONDEMO_API ADemoGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void InitProcess(int32 NewProcessIndex);
protected:
	virtual void BeginPlay() override;
private:
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	bool RequestHTTP(const FString URL);

	void InitPartsActorArray();

	void ResetPlayState();
	void SetSequence();

	void OnSequenceFinished();

	UFUNCTION(CallInEditor)
	void SetMaterialToTranslucent();
	UFUNCTION(CallInEditor)
	void SetMaterialToDefault();

public:
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FDele_Bool OnPlayChanged;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FDele_Bool OnProcessModeChanged;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FDele_Bool OnRepeatChanged;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FDele_Bool OnSoundChanged;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FDele_Bool OnDetailChanged;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FDele_Float OnSpeedChanged;
private:
	float PlaySpeed;
	bool bIsPlaying;
	bool bIsReadyToPlay;
	bool bIsRepeat;
	bool bPlaySound;
	bool bShowDetail;
	bool bIsPaused;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Meta = (AllowPrivateAccess = true))
	TArray<FCameraSetting> CameraSettingArray;

	ALevelSequenceActor* LevelSequenceActor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (AllowPrivateAccess = true))
	bool bIsAssembleMode = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (AllowPrivateAccess = true))
	int32 CurrentProcessIndex = 0;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = true))
	TArray<FProcessData> ProcessDataArray;

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true))
	FString AssembleString;
	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true))
	FString DisassembleString;
	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true))
	TMap<FString, int> PartsMap;

	// HTTP
	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true))
	FString APIURL;

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true))
	TSubclassOf<APartsActor> PartsActorClass;
};