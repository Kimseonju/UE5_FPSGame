// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSlot.h"
#include "Components/Image.h"
#include "COmponents/TextBlock.h"
#include "UE5_MultiPlayFPS/Interface/PickupItemInterface.h"
#include "UE5_MultiPlayFPS/Weapon/Weapon.h"
void UItemSlot::SetSlot(AWeapon* Weapon, int Ammo)
{
	if (Weapon)
	{
		FItemInfo ItemInfo = Weapon->GetItemInfo();

		AmmoCount->SetText(FText::FromString(FString::FromInt(Ammo)));
		if (ItemInfo.ItemImage)
		{
			WeaponImage->SetBrushFromTexture(ItemInfo.ItemImage);
			if (Weapon->GetWeaponState() == EWeaponState::EWS_Equipped)
			{
				WeaponImage->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
			}
			else
			{
				WeaponImage->SetColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 1.f));
			}
		}
	}
}
