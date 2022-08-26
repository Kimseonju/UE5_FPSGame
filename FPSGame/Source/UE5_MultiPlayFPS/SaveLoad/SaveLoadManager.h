// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SaveLoadManager.generated.h"

UCLASS()
class UE5_MULTIPLAYFPS_API ASaveLoadManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASaveLoadManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:
	bool bLoadCheck;
	class UGameSaveData* SaveData;

	TSubclassOf<class UGameSaveData> SaveDataClass;

	UPROPERTY(EditAnywhere)
	TArray<class ACheckPoint*> CheckPoints;
	TArray<FString> SaveSlot;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	void SaveGame();
	UFUNCTION(BlueprintCallable)
	void LoadGame();

};
