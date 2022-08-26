// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TurretAnimInstace.generated.h"

/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API UTurretAnimInstace : public UAnimInstance
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FRotator LookAtRotation;

public:
	void SetLookAtRotation(const FRotator& Rotation) { LookAtRotation = Rotation; }
	FRotator GetLookAtRotation()const { return LookAtRotation; }
};
