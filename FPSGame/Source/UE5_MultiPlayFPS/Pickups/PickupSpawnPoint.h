// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupSpawnPoint.generated.h"

UCLASS()
class UE5_MULTIPLAYFPS_API APickupSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	APickupSpawnPoint();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	class APickup* SpawnActor;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class APickup>> PickupClasses;
	
	void SpawnPickup();
	void SpawnPickupTimerFinish();
	UFUNCTION()
	void StartSpawnPickupTimer(AActor* DestroyedActor);
private:
	FTimerHandle SpawnPickupTimer;

	UPROPERTY(EditAnywhere)
	float SpawnTimeMin;
	UPROPERTY(EditAnywhere)
	float SpawnTimeMax;
public:	


};
