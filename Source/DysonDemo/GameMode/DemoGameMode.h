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
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText ProcessText;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<APartsActor*> PartsActorArray;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 SequenceIndex;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 CameraIndex;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FText> AssembleInstruction;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FText> DisassembleInstruction;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<UTexture2D*> DetailImageArray;
};

UCLASS()
class DYSONDEMO_API ADemoGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void OnSequenceFinished();

	UFUNCTION(BlueprintCallable)
	void InitProcess(int32 NewProcessIndex);
	UFUNCTION(BlueprintCallable)
	void SelectProcess(int32 NewProcessIndex);

	void MoveToNextProcess();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ShowInstruction(int32 Index);

	UFUNCTION(BlueprintCallable)
	bool GetPlaySound() const;

	UFUNCTION(BlueprintCallable)
	TArray<FProcessData> GetProcessDataArray() const;
	UFUNCTION(BlueprintCallable)
	FProcessData GetCurrentProcessData() const;

	UFUNCTION(BlueprintCallable)
	void CheckPause();

	// UI
	UFUNCTION(BlueprintCallable)
	void ToggleProcessMode();
	UFUNCTION(BlueprintCallable)
	void PressDetail();
	UFUNCTION(BlueprintCallable)
	void CPP_PressPlay();
	UFUNCTION(BlueprintCallable)
	void PressRepeat();
	UFUNCTION(BlueprintCallable)
	void PressSpeed();
	UFUNCTION(BlueprintCallable)
	void PressSound();

	// Blueprint
	UFUNCTION(BlueprintImplementableEvent)
	bool GetIsDetailShown() const;
	UFUNCTION(BlueprintImplementableEvent)
	void ShowDetailWidget() const;
	UFUNCTION(BlueprintImplementableEvent)
	void HideDetailWidget();
	UFUNCTION(BlueprintImplementableEvent)
	void BlinkDetailButton();
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void StopBlinkDetailButton();
	UFUNCTION(BlueprintImplementableEvent)
	void StopBlinkPlayButton() const;
	UFUNCTION(BlueprintImplementableEvent)
	void InitStateWidget() const;
	UFUNCTION(BlueprintImplementableEvent)
	void ReadyToPlay();
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SynchronizeWidget() const;
	
protected:
	virtual void BeginPlay() override;
private:
	void PlaySequence();
	void SetIsPlaying();
	void SetIsPause(bool bNewIsPause);
	void PauseSequence();
	// Called when Sequence Play is over 
	void EndAction();

	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	bool RequestHTTP(const FString URL);

	void InitPartsActorArray();

	void ResetPlayState();
	void SetSequence();

	

	UFUNCTION(CallInEditor)
	void SetMaterialToTranslucent();
	UFUNCTION(BlueprintCallable)
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
	FDele_Float OnPlayRateChanged;
private:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (AllowPrivateAccess = true))
	bool bIsCameraMoving = false;

	UPROPERTY(BlueprintReadWrite , EditAnywhere, Meta = (AllowPrivateAccess = true))
	float PlayRate = 1.0f;
	bool bIsPlayed = false;
	bool bIsPlaying = false;
	bool bIsReadyToPlay = false;
	bool bIsRepeat = false;
	bool bPlaySound = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (AllowPrivateAccess = true))
	bool bShowDetail = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (AllowPrivateAccess = true))
	bool bIsDetailBlinked = true;
	bool bIsPaused = false;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Meta = (AllowPrivateAccess = true))
	TArray<FCameraSetting> CameraSettingArray;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (AllowPrivateAccess = true))
	bool bIsAssembleMode = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (AllowPrivateAccess = true))
	int32 CurrentProcessIndex = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (AllowPrivateAccess = true))
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

	// Sound
	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true))
	USoundBase* BGMSound;
	UAudioComponent* BGM;
};