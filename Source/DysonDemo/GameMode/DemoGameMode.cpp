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
	UE_LOG(LogTemp, Warning, TEXT("ADemoGameMode::InitProcess : Called"));

	GetGameInstance<UMyGameInstance>()->ResetSequence(CurrentProcessIndex);

	CurrentProcessIndex = bIsAssembleMode ? ProcessDataArray.Num() - NewProcessIndex - 1 : NewProcessIndex;

	ResetPlayState();
	SetSequence();

	InitStateWidget();

	SynchronizeWidget();

	SetMaterialToTranslucent();
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
	InitProcess(0);
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

void ADemoGameMode::CPP_PressPlay()
{
	UE_LOG(LogTemp, Warning, TEXT("ADemoGameMode::CPP_PressPlay : Called"));
	SetMaterialToDefault();
	if (bIsPlaying)
	{
		if (bIsPaused)
		{
			SetIsPause(false);
			PlaySequence();
		}
		else
		{
			SetIsPause(true);
			PauseSequence();
		}
	}
	else
	{
		SetIsPlaying();
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
	GetGameInstance<UMyGameInstance>()->SetPlayRate(CurrentProcessIndex, PlayRate);

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
	UE_LOG(LogTemp, Warning, TEXT("ADemoGameMode::PlaySequence : Called"));

	if (bIsReadyToPlay)
	{
		GetGameInstance<UMyGameInstance>()->PlaySequence(CurrentProcessIndex, bIsAssembleMode);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ADemoGameMode::PlaySequence : Ready To Play"));
		bIsReadyToPlay = true;
		ReadyToPlay();
	}
}

void ADemoGameMode::SetIsPlaying()
{
	bIsPlaying = !bIsPlaying;
	OnPlayChanged.Broadcast(bIsPlaying && !bIsPaused);
}

void ADemoGameMode::SetIsPause(bool bNewIsPause)
{
	bIsPaused = bNewIsPause;
	OnPlayChanged.Broadcast(bIsPlaying && !bIsPaused);
}

void ADemoGameMode::PauseSequence()
{
	GetGameInstance<UMyGameInstance>()->Pause(CurrentProcessIndex);
}

void ADemoGameMode::BeginPlay()
{
	Super::BeginPlay();

	GetGameInstance<UMyGameInstance>()->InitLevelSequence(this);

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
	UE_LOG(LogTemp, Warning, TEXT("ADemoGameMode::EndAction : Called"));
	if (bIsRepeat)
	{
		PlaySequence();
	}
	else
	{
		if (bIsAssembleMode)
		{
			if (CurrentProcessIndex > 0)
			{
				InitProcess(ProcessDataArray.Num() - CurrentProcessIndex);
				SynchronizeWidget();
				CPP_PressPlay();
			}
			else
			{
				// End Of Process
				ResetPlayState();
			}
		}
		else
		{
			if (CurrentProcessIndex < ProcessDataArray.Num() - 1)
			{
				InitProcess(CurrentProcessIndex + 1);
				SynchronizeWidget();
				CPP_PressPlay();
			}
			else
			{
				// End Of Process
				ResetPlayState();
			}
		}
		

		/*if (bIsAssembleMode)
		{
			if (CurrentProcessIndex > 0)
			{
				
				InitProcess(ProcessDataArray.Num() - CurrentProcessIndex);
				PressPlay();
			}
			else
			{
				ResetPlayState();
			}
		}
		else
		{
			if (CurrentProcessIndex < ProcessDataArray.Num() - 1)
			{
				InitProcess(CurrentProcessIndex + 1);
				PressPlay();
			}
			else
			{
				ResetPlayState();
			}
		}*/
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
		UE_LOG(LogTemp, Error, TEXT("ADemoGameMode::InitPartsActorArray : There's No PartsActor"));
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
	UE_LOG(LogTemp, Warning, TEXT("ADemoGameMode::SetSequence : Called"));

	GetGameInstance<UMyGameInstance>()->SetSequence(CurrentProcessIndex, PlayRate);

	/*if (LevelSequenceActor->IsValidLowLevelFast())
	{
		FMovieSceneSequencePlaybackParams Params;
		Params.UpdateMethod = EUpdatePositionMethod::Play;
		Params.Frame.FrameNumber = LevelSequenceActor->SequencePlayer->GetStartTime().Time.FrameNumber;
		LevelSequenceActor->SequencePlayer->SetPlaybackPosition(Params);
		LevelSequenceActor->Destroy();
	}

	if (ULevelSequence* LevelSequence = GetGameInstance<UMyGameInstance>()->GetLevelSequence(ProcessDataArray[CurrentProcessIndex].SequenceIndex))
	{
		ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), LevelSequence, FMovieSceneSequencePlaybackSettings(), LevelSequenceActor);
		LevelSequenceActor->SequencePlayer->SetPlayRate(PlayRate);
		LevelSequenceActor->SequencePlayer->OnFinished.AddDynamic(this, &ADemoGameMode::OnSequenceFinished);
		FMovieSceneSequencePlaybackParams Params;
		Params.Frame.FrameNumber = LevelSequenceActor->SequencePlayer->GetStartTime().Time.FrameNumber;
		LevelSequenceActor->SequencePlayer->SetPlaybackPosition(Params);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ADemoGameMode::SetSequence : LevelSequence is not Valid"));
	}*/
}

void ADemoGameMode::OnSequenceFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("ADemoGameMode::OnSequenceFinished : Called"));
	if (bShowDetail && ProcessDataArray[CurrentProcessIndex].DetailImageArray.Num() > 0)
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
	for (int i = 0; i < ProcessDataArray.Num(); ++i)
	{
		if (i < CurrentProcessIndex)
		{
			for (APartsActor* PartsActor : ProcessDataArray[i].PartsActorArray)
			{
				PartsActor->SetActorHiddenInGame(true);
			}
		}
		else if (i == CurrentProcessIndex)
		{
			for (APartsActor* PartsActor : ProcessDataArray[i].PartsActorArray)
			{
				PartsActor->SetMaterialToDefault();
			}
		}
		else
		{
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
		else if (i == CurrentProcessIndex)
		{
			for (APartsActor* PartsActor : ProcessDataArray[i].PartsActorArray)
			{
				if (bIsAssembleMode)
				{
					if (CurrentProcessIndex == ProcessDataArray.Num() - 1)
					{
						PartsActor->SetMaterialToDefault();
					}
					else
					{
						PartsActor->SetActorHiddenInGame(true);
					}
				}
				else
				{
					PartsActor->SetMaterialToDefault();
				}
					
			}
		}
		else
		{
			for (APartsActor* PartsActor : ProcessDataArray[i].PartsActorArray)
			{
				PartsActor->SetMaterialToDefault();
			}
		}
	}
	GetGameInstance<UMyGameInstance>()->SetPosition(CurrentProcessIndex, bIsAssembleMode);
}