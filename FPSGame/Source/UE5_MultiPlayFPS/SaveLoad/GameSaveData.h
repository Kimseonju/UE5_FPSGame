// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameSaveData.generated.h"

/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API UGameSaveData : public USaveGame
{
	GENERATED_BODY()
	
public:
	FTransform PlayerTransform;
	TArray<class AWeapon*> WeaponItems;
	

};
