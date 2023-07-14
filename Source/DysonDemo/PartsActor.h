// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PartsActor.generated.h"

class UStaticMeshComponent;
UCLASS()
class DYSONDEMO_API APartsActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APartsActor();

	UFUNCTION(CallInEditor)
	void SetMaterialToDefault();
	UFUNCTION(CallInEditor)
	void SetMaterialToTranslucent();

	int32 GetProcessIndex() const;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:
	void SaveDefaultMaterial();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override; 
private:
	UPROPERTY(EditInstanceOnly, Meta = (AllowPrivateAccess = true))
	int32 ProcessIndex;
	
	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true))
	TArray<UMaterialInterface*> DefaultMaterialArray;

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true))
	UMaterialInstance* TranslucentMaterial;
};