// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopPickupBuffWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UShopPickupBuffWidget::InitWidget(UTexture2D* Image, FText FnName, FText FPrice, FText FIndex, EPickupType EType)
{
	PickUpImage->SetBrushFromTexture(Image);

	Name->SetText(FnName);
	Index->SetText(FIndex);
	Price->SetText(FPrice);
	Type = EType;
}
