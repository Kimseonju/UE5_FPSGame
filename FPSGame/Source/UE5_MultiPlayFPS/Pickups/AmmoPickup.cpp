// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoPickup.h"
#include "UE5_MultiPlayFPS/Character/MultiPlayCharacter.h"
#include "UE5_MultiPlayFPS/MultiPlayComponents/CombatComponent.h"
#include "UE5_MultiPlayFPS/Widget/HUD/AmmoPickUpWidget.h"

void AAmmoPickup::BeginPlay()
{
	Super::BeginPlay();

	ItemType = EItemType::EIT_Ammo;
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
		UAmmoPickupWidget* Widget = Cast<UAmmoPickupWidget>(PickupWidget->GetWidget());
		if (Widget)
		{
			ItemInfo.Ammo = Ammo;
			Widget->SetInit(ItemInfo, ItemType);
			PickupWidget->SetWidget(Widget);
		}
	}
}

void AAmmoPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	//
	//AMultiPlayCharacter* PlayCharacter = Cast<AMultiPlayCharacter>(OtherActor);
	//
	//if (PlayCharacter)
	//{
	//	UCombatComponent* Combat = PlayCharacter->GetCombat();
	//	if (Combat)
	//	{
	//		Combat->PickupAmmo(WeaponType, Ammo);
	//
	//	}
	//}
	//Destroy();
}

void AAmmoPickup::EquippedEvent(class AMultiPlayCharacter* Player)
{
	if (Player)
	{
		UCombatComponent* Combat = Player->GetCombat();
		if (Combat)
		{
			Combat->PickupAmmo(WeaponType, Ammo);

		}
	}
	Destroy();
}
