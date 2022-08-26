// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShopAmmoItem.generated.h"

/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API UShopAmmoItem : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* WeaponImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* WeaponName;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* WeaponAmmo;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* WeaponPrice;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* Button;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EWeaponType Type;
	UFUNCTION(BlueprintCallable)
		void InitWidget(UTexture2D* Image, FText FWeaponName, FText FWeaponPrice, FText Ammo,EWeaponType EType);
};
