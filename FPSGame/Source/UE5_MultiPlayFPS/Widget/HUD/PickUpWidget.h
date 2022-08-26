// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UE5_MultiPlayFPS/Interface/PickupItemInterface.h"
#include "PickUpWidget.generated.h"

class UTextBlock;
class UImage;
UCLASS()
class UE5_MULTIPLAYFPS_API UPickUpWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemType;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemName;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemText;
	UPROPERTY(meta = (BindWidget))
		UImage* ItemImage;

	void SetInit(const FItemInfo& Info, EItemType Type);
};
