// Fill out your copyright notice in the Description page of Project Settings.


#include "../GameMode/DemoGameMode.h"

#include <Kismet/GameplayStatics.h>
#include <LevelSequenceActor.h>
#include <LevelSequencePlayer.h>
#include <LevelSequence.h>
#include <Kismet/GameplayStatics.h>
#include <Components/AudioComponent.h>

#include "../PartsActor.h"
#include "../GameInstance/MyGameInstance.h"

bool ADemoGameMode::RequestHTTP(const FString URL)
{
	if (URL.IsEmpty())
	{
		UE_LOG(LogTemp, Display, TEXT("ADemoGameMode::RequestHTTP : URL is Empty"));
		return false;
	}
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(URL);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ADemoGameMode::OnResponseReceived);
	HttpRequest->SetVerb("GET");


	return HttpRequest->ProcessRequest();
}

void ADemoGameMode::InitProcess(int32 NewProcessIndex)
{
	CurrentProcessIndex = bIsAssembleMode ? ProcessDataArray.Num() - NewProcessIndex - 1 : NewProcessIndex;

	SetMaterialToTranslucent();

	ResetPlayState();
	SetSequence();

	InitStateWidget();
}


bool ADemoGameMode::GetPlaySound() const
{
	return bPlaySound;
}

TArray<FProcessData> ADemoGameMode::GetProcessDataArray() const
{
	return ProcessDataArray;
}

FProcessData ADemoGameMode::GetCurrentProcessData() const
{
	return ProcessDataArray[CurrentProcessIndex];
}

void ADemoGameMode::ToggleProcessMode()
{
	bIsAssembleMode = !bIsAssembleMode;
	OnProcessModeChanged.Broadcast(bIsAssembleMode);
}

void ADemoGameMode::PressDetail()
{
	if (GetIsDetailShown())
	{
		HideDetailWidget();
		EndAction();
	}
	else
	{
		bShowDetail = !bShowDetail;
		OnDetailChanged.Broadcast(bShowDetail);
	}
}

void ADemoGameMode::PressPlay()
{
	SetMaterialToDefault();
	if (bIsPlaying)
	{
		SetIsPause();
		if (bIsPaused)
		{
			Pause();
		}
		else
		{
			PlaySequence();
		}
	}
	else
	{
		PlaySequence();
	}
}

void ADemoGameMode::PressRepeat()
{
	bIsRepeat = !bIsRepeat;
	OnRepeatChanged.Broadcast(bIsRepeat);
}

void ADemoGameMode::PressSpeed()
{
	if (PlayRate == 1.0f)
	{
		PlayRate = 0.5f;
	}
	else if (PlayRate == 0.5f)
	{
		PlayRate = 0.25f;
	}
	else if (PlayRate == 0.25f)
	{
		PlayRate = 1.0f;
	}
	if (LevelSequenceActor->IsValidLowLevelFast())
	{
		LevelSequenceActor->SequencePlayer->SetPlayRate(PlayRate);
	}

	OnPlayRateChanged.Broadcast(PlayRate);
}

void ADemoGameMode::PressSound()
{
	bPlaySound = !bPlaySound;
	OnSoundChanged.Broadcast(bPlaySound);
	if (BGM)
	{
		BGM->SetPaused(!bPlaySound);
	}
}

void ADemoGameMode::CheckPause()
{
	if (!bIsPaused)
	{
		PlaySequence();
	}
}

void ADemoGameMode::PlaySequence()
{
	SetIsPlaying();

	if (bIsReadyToPlay)
	{
		bIsAssembleMode ? LevelSequenceActor->SequencePlayer->PlayReverse() : LevelSequenceActor->SequencePlayer->Play();
	}
	else
	{
		bIsReadyToPlay = true;
		ReadyToPlay();
	}
}

void ADemoGameMode::SetIsPlaying()
{
	bIsPlaying = !bIsPlaying;
	OnPlayChanged.Broadcast(bIsPlaying && !bIsPaused);
}

void ADemoGameMode::SetIsPause()
{
	bIsPaused = !bIsPaused;
	OnPlayChanged.Broadcast(bIsPlaying && !bIsPaused);
}

void ADemoGameMode::Pause()
{
	LevelSequenceActor->SequencePlayer->Pause();
}

void ADemoGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (PartsActorClass)
	{
		InitPartsActorArray();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ADemoGameMode::BeginPlay : PartsActorClass is not Valid"));
	}

	if (!APIURL.IsEmpty())
	{
		RequestHTTP(APIURL);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ADemoGameMode::BeginPlay : APIURL is Empty"));
	}

	if (BGMSound)
	{
		BGM = UGameplayStatics::SpawnSound2D(GetWorld(), BGMSound);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ADemoGameMode::BeginPlay : BGMSound is not Valid"));
	}
}

void ADemoGameMode::EndAction()
{
	if (bIsRepeat)
	{
		FMovieSceneSequencePlaybackParams Params;
		if (bIsAssembleMode)
		{
			Params.Frame.FrameNumber = LevelSequenceActor->SequencePlayer->GetEndTime().Time.FrameNumber;
			LevelSequenceActor->SequencePlayer->SetPlaybackPosition(Params);
			LevelSequenceActor->SequencePlayer->PlayReverse();
		}
		else
		{
			Params.Frame.FrameNumber = LevelSequenceActor->SequencePlayer->GetStartTime().Time.FrameNumber;
			LevelSequenceActor->SequencePlayer->SetPlaybackPosition(Params);
			LevelSequenceActor->SequencePlayer->Play();
		}
	}
	else
	{
		ResetPlayState();
	}
}

void ADemoGameMode::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (!bConnectedSuccessfully)
	{
		UE_LOG(LogTemp, Error, TEXT("ADemoGameMode::OnResponseReceived : API Connect Failed"));
		return; 
	}
	//UE_LOG(LogTemp, Display, TEXT("Response : %s"), *Response->GetContentAsString());
	TSharedPtr<FJsonObject> ResponseObj;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	FJsonSerializer::Deserialize(Reader, ResponseObj);
	// Result
	TSharedPtr<FJsonObject> ResultObj = ResponseObj->GetObjectField("result");
	if (ResultObj.IsValid())
	{
		TArray<TSharedPtr<FJsonValue>> DataArray = ResultObj->GetArrayField("data");
		if (DataArray.Num() > 0)
		{

			int32 ProcessIndex = -1;
			for (int i = 1; i < DataArray.Num(); ++i)
			{
				TSharedPtr<FJsonObject> DataObj = DataArray[i]->AsObject();
				FString ProcessText = DataObj->GetStringField("ProcessText");
				FString NumText = DataObj->GetStringField("NumText");
				FString CamText = DataObj->GetStringField("CamText");

				// 분해 / 조립 구분
				if (ProcessText.Compare(AssembleString) == 0)
				{
					ProcessDataArray[*PartsMap.Find(NumText)].AssembleInstruction.Add(FText::FromString(CamText));
				}
				else
				{
					ProcessDataArray[*PartsMap.Find(NumText)].DisassembleInstruction.Add(FText::FromString(CamText));
				}
			}
		}
	}
}

void ADemoGameMode::InitPartsActorArray()
{
	TArray<AActor*> PartsActorArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), PartsActorClass, PartsActorArray);

	if (PartsActorArray.Num() > 0)
	{
		for (AActor* Iter : PartsActorArray)
		{
			if (APartsActor* PartsActor = Cast<APartsActor>(Iter))
			{
				ProcessDataArray[PartsActor->GetProcessIndex()].PartsActorArray.Add(PartsActor);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ADemoGameMode::BeginPlay : There's No PartsActor"));
	}
}

void ADemoGameMode::ResetPlayState()
{
	bIsPlaying = false;
	bIsReadyToPlay = false;
	bIsPaused = false;

	OnPlayChanged.Broadcast(false);
}

void ADemoGameMode::SetSequence()
{
	if (LevelSequenceActor->IsValidLowLevelFast())
	{
		LevelSequenceActor->Destroy();
	}

	if (ULevelSequence* LevelSequence = GetGameInstance<UMyGameInstance>()->GetLevelSequence(ProcessDataArray[CurrentProcessIndex].SequenceIndex))
	{
		ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), LevelSequence, FMovieSceneSequencePlaybackSettings(), LevelSequenceActor);
		LevelSequenceActor->SequencePlayer->SetPlayRate(PlayRate);
		LevelSequenceActor->SequencePlayer->OnFinished.AddDynamic(this, &ADemoGameMode::OnSequenceFinished);
		FMovieSceneSequencePlaybackParams Params;
		Params.Frame.FrameNumber = bIsAssembleMode ? LevelSequenceActor->SequencePlayer->GetEndTime().Time.FrameNumber : LevelSequenceActor->SequencePlayer->GetStartTime().Time.FrameNumber;
		LevelSequenceActor->SequencePlayer->SetPlaybackPosition(Params);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ADemoGameMode::SetSequence : LevelSequence is not Valid"));
	}
}

void ADemoGameMode::OnSequenceFinished()
{
	if (bShowDetail)
	{
		ShowDetailWidget();
	}
	else
	{
		EndAction();
	}
}

void ADemoGameMode::SetMaterialToTranslucent()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Translucent : %s"), *ProcessDataArray[CurrentProcessIndex].ProcessText.ToString()));
	for (APartsActor* PartsActor : ProcessDataArray[CurrentProcessIndex].PartsActorArray)
	{
		PartsActor->SetMaterialToDefault();
	}
	for (int i = 0; i < ProcessDataArray.Num(); ++i)
	{
		if (i < CurrentProcessIndex)
		{
			for (APartsActor* PartsActor : ProcessDataArray[i].PartsActorArray)
			{
				PartsActor->SetActorHiddenInGame(true);
			}
		}
		else if (i > CurrentProcessIndex)
		{
			// i > CurrentProcessIndex

			for (APartsActor* PartsActor : ProcessDataArray[i].PartsActorArray)
			{
				PartsActor->SetMaterialToTranslucent();
			}
		}
	}
}

void ADemoGameMode::SetMaterialToDefault()
{
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("Default : %s"), *ProcessDataArray[CurrentProcessIndex].ProcessText.ToString()));

	for (int i = 0; i < ProcessDataArray.Num(); ++i)
	{
		if (i < CurrentProcessIndex)
		{
			for (APartsActor* PartsActor : ProcessDataArray[i].PartsActorArray)
			{
				PartsActor->SetActorHiddenInGame(true);
			}
		}
		else
		{
			// i > CurrentProcessIndex

			for (APartsActor* PartsActor : ProcessDataArray[i].PartsActorArray)
			{
				PartsActor->SetMaterialToDefault();
			}
		}
	}
}