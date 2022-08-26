// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopWeaponItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
void UShopWeaponItem::InitWidget(FWeaponInfo Info)
{
	WeaponImage->SetBrushFromTexture(Info.Image);

	//ShopWeaponItem3->InitWidget(Info3.Image, FText::FromString(Info3.Name), FText::FromString(FString::FromInt(Info3.Price)), Info3.Type, Info3.WeaponClass);
	WeaponName->SetText(FText::FromString(Info.Name));
	WeaponPrice->SetText(FText::FromString(FString::FromInt(Info.Price)));
	WeaponClass = Info.WeaponClass;
	Type = Info.Type;
}
