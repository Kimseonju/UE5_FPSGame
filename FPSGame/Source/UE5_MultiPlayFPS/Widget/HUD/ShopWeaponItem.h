// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UE5_MultiPlayFPS/Weapon/Weapon.h"

#include "ShopWeaponItem.generated.h"

/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API UShopWeaponItem : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	class UImage* WeaponImage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* WeaponName;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* WeaponPrice;

	UPROPERTY(meta = (BindWidget))
	class UButton* Button;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AWeapon> WeaponClass;
	UFUNCTION(BlueprintCallable)
	void InitWidget(FWeaponInfo Info);
};
