// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionNPC_Interface.generated.h"

UINTERFACE(MinimalAPI)
class UInteractionNPC_Interface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UE5_MULTIPLAYFPS_API IInteractionNPC_Interface
{
	GENERATED_BODY()

public:
	void OnInteraction(AMultiPlayCharacter* Character);
};
