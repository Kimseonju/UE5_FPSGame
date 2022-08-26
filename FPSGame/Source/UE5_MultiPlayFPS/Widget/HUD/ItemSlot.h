// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemSlot.generated.h"

/**
 * 
 */
class UTextBlock;
class UImage;
UCLASS()
class UE5_MULTIPLAYFPS_API UItemSlot : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SlotNumber;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* AmmoCount;
	UPROPERTY(meta = (BindWidget))
	UImage* WeaponImage;

	void SetSlot(class AWeapon* Weapon, int Ammo);
};
