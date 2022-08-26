// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UE5_MultiPlayFPS/Interface/PickupItemInterface.h"
#include "AmmoPickupWidget.generated.h"

class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API UAmmoPickupWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
		UTextBlock* ItemType;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* ItemName;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* ItemCount;

	UPROPERTY(meta = (BindWidget))
		UImage* ItemImage;
	UPROPERTY(meta = (BindWidget))
		UImage* ItemSubImage;

	void SetInit(const FItemInfo& Info, EItemType Type);
};
