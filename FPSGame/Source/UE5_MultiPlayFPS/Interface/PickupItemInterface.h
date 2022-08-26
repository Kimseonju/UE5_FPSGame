// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Components/Image.h"
#include "PickupItemInterface.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_Weapon UMETA(DisplayName = "Weapon"),
	EIT_Ammo UMETA(DisplayName = "Ammo"),
	EIT_Item UMETA(DisplayName = "Item")
};

USTRUCT(BlueprintType)
struct FItemInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
	FString Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
	UTexture2D* ItemImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
	UTexture2D* ItemImageSub;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
	FString Text;
	int32 Ammo;

};
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPickupItemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UE5_MULTIPLAYFPS_API IPickupItemInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void ShowPickupWidget(bool bShowWidget);
	virtual void EquippedEvent(class AMultiPlayCharacter* Player);
	bool bOverlap;
	EItemType ItemType;
};
