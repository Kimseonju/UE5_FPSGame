// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UE5_MultiPlayFPS/Weapon/Weapon.h"
#include "ShopCustomBar.generated.h"

/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API UShopCustomBar : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UShopWeaponItem* ShopWeaponItem1;

	UPROPERTY(meta = (BindWidget))
	class UShopWeaponItem* ShopWeaponItem2;

	UPROPERTY(meta = (BindWidget))
	class UShopWeaponItem* ShopWeaponItem3;

	UFUNCTION(BlueprintCallable)
	void InitWidget(
		FWeaponInfo Info1,
		FWeaponInfo Info2,
		FWeaponInfo Info3);
};
