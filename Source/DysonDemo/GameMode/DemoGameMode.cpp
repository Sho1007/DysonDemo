// Fill out your copyright notice in the Description page of Project Settings.


#include "../GameMode/DemoGameMode.h"

void ADemoGameMode::BeginPlay()
{
	Super::BeginPlay();

	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->SetURL("https://home.jsflux.co.kr:8443/api/sample/getProductData/dyson/0");
	Request->OnProcessRequestComplete().BindUObject(this, &ADemoGameMode::OnResponseReceived);
	Request->SetVerb("GET");
	Request->ProcessRequest();
}

void ADemoGameMode::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{	
	TSharedPtr<FJsonObject> ResponseObj;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	FJsonSerializer::Deserialize(Reader, ResponseObj);
	UE_LOG(LogTemp, Display, TEXT("Response : %s"), *Response->GetContentAsString());
	UE_LOG(LogTemp, Display, TEXT("Data Array Num: %d"), ResponseObj->GetArrayField("Data").Num());
}