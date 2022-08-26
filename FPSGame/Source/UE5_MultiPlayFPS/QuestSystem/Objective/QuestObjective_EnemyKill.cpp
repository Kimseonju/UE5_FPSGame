// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestObjective_EnemyKill.h"
#include "UE5_MultiPlayFPS/Character/Enemy.h"
#include "UE5_MultiPlayFPS/FieldManager.h"
#include "UE5_MultiPlayFPS/Widget/HUD/CharacterOverlay.h"
#include "UE5_MultiPlayFPS/PlayerController/MultiPlayPlayerController.h"

void UQuestObjective_EnemyKill::ActivateObjective()
{
	Super::ActivateObjective();
	Enemys = FieldManager->GetAllEnemy();
	for (AActor* Actor : Enemys)
	{
		AEnemy* Enemy = (AEnemy*)Actor;
		Enemy->OnTakeAnyDamage.AddDynamic(this, &UQuestObjective_EnemyKill::ReceiveDamageEvent);
	}
}
void UQuestObjective_EnemyKill::ReceiveDamageEvent(AActor* DamageActor, float Damage, 
	const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser)
{
	AEnemy* Enemy=Cast<AEnemy>(DamageActor);
	if (!Enemy) return;
	if (0.f >= Enemy->GetHealth())
	{
		Enemys.Remove(Enemy);
		Enemy->OnTakeAnyDamage.RemoveDynamic(this, &UQuestObjective_EnemyKill::ReceiveDamageEvent);
		EnemyKill++;
		if (EnemyKill >= EnemyKillMax)
		{
			for (AActor* Actor : Enemys)
			{
				AEnemy* removeDynamicEnemy = (AEnemy*)Actor;
				removeDynamicEnemy->OnTakeAnyDamage.RemoveDynamic(this, &UQuestObjective_EnemyKill::ReceiveDamageEvent);
			}
			QuestClear = true;
			FString EventText = Name.ToString();
			EventText = EventText + "Clear";
			PlayerController->GetCharacterOverlay()->QuestTextEvent(FText::FromString(EventText));
			OnCompletedDelegate.Broadcast(this);
		}
		else
			PlayerController->GetCharacterOverlay()->QuestTextEvent(FText::FromName(GetObjectiveName()));
	}
}



FName UQuestObjective_EnemyKill::GetObjectiveName()
{
	FString str=Name.ToString();
	str = str + " "+FString::FromInt(EnemyKill) + " / " + FString::FromInt(EnemyKillMax);
	return FName(str);
}

void UQuestObjective_EnemyKill::PrevInit(class AMultiPlayPlayerController* Controller, FName ObjectName, int iEnemyKillMax, class AFieldManager* Manager)
{
	PlayerController = Controller;
	Name = ObjectName;
	EnemyKillMax = iEnemyKillMax;
	FieldManager = Manager;
}
