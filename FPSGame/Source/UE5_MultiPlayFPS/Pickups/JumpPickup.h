// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "JumpPickup.generated.h"

/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API AJumpPickup : public APickup
{
	GENERATED_BODY()

public:
	AJumpPickup();
protected:

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UPROPERTY(EditAnywhere)
	float JumpVelocity;

	UPROPERTY(EditAnywhere)
		float JumpBuffTime;
};
