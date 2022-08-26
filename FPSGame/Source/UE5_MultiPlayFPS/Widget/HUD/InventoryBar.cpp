// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryBar.h"
#include "Components/Image.h"
#include "COmponents/TextBlock.h"
#include "UE5_MultiPlayFPS/MultiPlayComponents/CombatComponent.h"
#include "UE5_MultiPlayFPS/Weapon/Weapon.h"
#include "ItemSlot.h"


void UInventoryBar::NativeConstruct()
{
	Super::NativeConstruct();
	Slot1->SlotNumber->SetText(FText::FromString("1"));
	Slot2->SlotNumber->SetText(FText::FromString("2"));
	Slot3->SlotNumber->SetText(FText::FromString("3"));
	Slot4->SlotNumber->SetText(FText::FromString("4"));
	Slot5->SlotNumber->SetText(FText::FromString("5"));
	Slots.Add(Slot1);
	Slots.Add(Slot2);
	Slots.Add(Slot3);
	Slots.Add(Slot4);
	Slots.Add(Slot5);
}

void UInventoryBar::UpdateSlot(UCombatComponent* Combat)
{
	if (Combat)
	{
		TArray<AWeapon*> Weapon = Combat->GetInventory();
		for (int i = 0; i < Weapon.Num(); ++i)
		{
			if (Weapon[i] == nullptr) continue;
			Slots[i]->SetSlot(Weapon[i], Combat->GetCarriedAmmo(Weapon[i]->GetWeaponType()) + Weapon[i]->GetAmmo());
		}
		
	}
}
