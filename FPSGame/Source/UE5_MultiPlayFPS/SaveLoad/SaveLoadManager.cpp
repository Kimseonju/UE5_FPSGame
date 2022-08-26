// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveLoadManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameSaveData.h"
#include "CheckPoint.h"
#include "UE5_MultiPlayFPS/Character/MultiPlayCharacter.h"
#include "UE5_MultiPlayFPS/Weapon/Weapon.h"
#include "UE5_MultiPlayFPS/MultiPlayComponents/CombatComponent.h"

ASaveLoadManager::ASaveLoadManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASaveLoadManager::BeginPlay()
{
	Super::BeginPlay();
	for (ACheckPoint* Point : CheckPoints)
	{
		Point->SetSaveLoadManager(this);
	}
}

void ASaveLoadManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASaveLoadManager::SaveGame()
{
	if (SaveData ==nullptr)
	{
		if (SaveDataClass == nullptr) return;

		SaveData = Cast< UGameSaveData>(UGameplayStatics::CreateSaveGameObject(SaveDataClass));
	}

	AMultiPlayCharacter* Character = Cast< AMultiPlayCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (Character)
	{

		AMultiPlayPlayerState* State = Character->GetPlayerState<AMultiPlayPlayerState>();

		if (State)
		{
			State->Save(Character->GetActorTransform());
		}

		SaveData->WeaponItems = Character->GetCombat()->GetInventory();
		SaveData->PlayerTransform = Character->GetActorTransform();

		UGameplayStatics::SaveGameToSlot(SaveData, TEXT("SaveData"), 0);
	}
}

void ASaveLoadManager::LoadGame()
{
	if (UGameplayStatics::DoesSaveGameExist(TEXT("SaveData"), 0))
	{
		SaveData =Cast<UGameSaveData>(UGameplayStatics::LoadGameFromSlot(TEXT("SaveData"), 0));
		if (SaveData)
		{
			AMultiPlayCharacter* Character = Cast< AMultiPlayCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
			if (Character)
			{
				for (AWeapon* Weapon : SaveData->WeaponItems)
				{
					Character->GetCombat()->EquipWeapon(Weapon);
				}
				Character->SetActorLocationAndRotation(SaveData->PlayerTransform.GetLocation(), SaveData->PlayerTransform.Rotator(), false, false);
			}
		}
	}
}

