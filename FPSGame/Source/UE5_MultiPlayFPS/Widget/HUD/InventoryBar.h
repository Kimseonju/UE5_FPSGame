// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryBar.generated.h"

class UItemSlot;
/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API UInventoryBar : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
public:

	UPROPERTY(meta = (BindWidget))
	UItemSlot* Slot1;
	UPROPERTY(meta = (BindWidget))
	UItemSlot* Slot2;
	UPROPERTY(meta = (BindWidget))
	UItemSlot* Slot3;
	UPROPERTY(meta = (BindWidget))
	UItemSlot* Slot4;
	UPROPERTY(meta = (BindWidget))
		UItemSlot* Slot5;
	TArray<UItemSlot*> Slots;
	void UpdateSlot(class UCombatComponent* Combat);

};
