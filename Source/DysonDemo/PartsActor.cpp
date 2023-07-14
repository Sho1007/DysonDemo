// Fill out your copyright notice in the Description page of Project Settings.


#include "PartsActor.h"
#include <Components/StaticMeshComponent.h>

// Sets default values
APartsActor::APartsActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
}

void APartsActor::SetMaterialToDefault()
{
	SetActorHiddenInGame(false);
	for (int i = 0; i < DefaultMaterialArray.Num(); ++i)
	{
		StaticMeshComponent->SetMaterial(i, DefaultMaterialArray[i]);
	}
}

void APartsActor::SetMaterialToTranslucent()
{
	SetActorHiddenInGame(false);
	for (int i = 0; i < DefaultMaterialArray.Num(); ++i)
	{
		StaticMeshComponent->SetMaterial(i, TranslucentMaterial);
	}
}

int32 APartsActor::GetProcessIndex() const
{
	return ProcessIndex;
}

// Called when the game starts or when spawned
void APartsActor::BeginPlay()
{
	Super::BeginPlay();
	
	SaveDefaultMaterial();
}

void APartsActor::SaveDefaultMaterial()
{
	if (StaticMeshComponent && StaticMeshComponent->GetStaticMesh())
	{
		DefaultMaterialArray = StaticMeshComponent->GetMaterials();
	}
}

// Called every frame
void APartsActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

