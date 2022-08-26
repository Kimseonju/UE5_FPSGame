// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpWidget.h"
#include "Components/Image.h"
#include "COmponents/TextBlock.h"
void UPickUpWidget::SetInit(const FItemInfo& Info, EItemType Type)
{
	if (Info.ItemImage)
	{
		ItemImage->SetBrushFromTexture(Info.ItemImage);
	}
	ItemType->SetText(FText::FromString(Info.Type));
	ItemName->SetText(FText::FromString(Info.Name));
	ItemText->SetText(FText::FromString(Info.Text));
	
	
}
