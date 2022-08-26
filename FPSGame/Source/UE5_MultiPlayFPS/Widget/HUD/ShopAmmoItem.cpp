// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopAmmoItem.h"

void UShopAmmoItem::InitWidget(UTexture2D* Image, FText FWeaponName, FText FWeaponPrice, FText Ammo, EWeaponType EType)
{

	WeaponImage->SetBrushFromTexture(Image);

	WeaponName->SetText(FWeaponName);
	WeaponPrice->SetText(FWeaponPrice);
	WeaponAmmo->SetText(Ammo);
	Type = EType;
}
