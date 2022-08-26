// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoPickupWidget.h"
#include "Components/Image.h"
#include "COmponents/TextBlock.h"

void UAmmoPickupWidget::SetInit(const FItemInfo& Info, EItemType Type)
{
	if (Info.ItemImage)
	{
		ItemImage->SetBrushFromTexture(Info.ItemImage);
	}

	ItemType->SetText(FText::FromString(Info.Type));

	ItemName->SetText(FText::FromString(Info.Name));
	ItemCount->SetText(FText::FromString(FString::FromInt(Info.Ammo)));
	if (Info.ItemImageSub)
	{
		ItemSubImage->SetBrushFromTexture(Info.ItemImageSub);
	}
}
