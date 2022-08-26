// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CheckPoint.generated.h"

UCLASS()
class UE5_MULTIPLAYFPS_API ACheckPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	ACheckPoint();

protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess="true"))
	class USceneComponent* RootComp;
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* Box;
	class ASaveLoadManager* SaveLoadManager;
public:	
	void SetSaveLoadManager(class ASaveLoadManager* Manager) { SaveLoadManager = Manager; }
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void CollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
