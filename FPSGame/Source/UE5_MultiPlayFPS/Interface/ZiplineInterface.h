// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ZiplineInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UZiplineInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UE5_MULTIPLAYFPS_API IZiplineInterface
{
	GENERATED_BODY()


public:

	virtual void SetZipline(bool Enable, const FVector& Start, const FVector& End);
};
