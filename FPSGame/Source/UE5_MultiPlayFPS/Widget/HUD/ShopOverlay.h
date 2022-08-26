// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShopOverlay.generated.h"

/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API UShopOverlay : public UUserWidget
{
	GENERATED_BODY()

	class UListView* WeaponList;

};
