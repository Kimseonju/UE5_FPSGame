// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShopPickupBuffWidget.generated.h"


UENUM(BlueprintType)
enum class EPickupType :uint8
{
	EPT_Heal,
	EPT_Shield
};
/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API UShopPickupBuffWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* PickUpImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* Name;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* Index;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* Price;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* Button;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPickupType Type;

	UFUNCTION(BlueprintCallable)
		void InitWidget(UTexture2D* Image, FText FnName, FText FPrice, FText FIndex, EPickupType EType);
};
