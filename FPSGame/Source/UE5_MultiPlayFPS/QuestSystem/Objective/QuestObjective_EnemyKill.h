// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestObjective.h"
#include "QuestObjective_EnemyKill.generated.h"

/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API UQuestObjective_EnemyKill : public UQuestObjective
{
	GENERATED_BODY()

	TArray <AActor*> Enemys;
	class AFieldManager* FieldManager;
	int EnemyKill;
	UPROPERTY(EditAnywhere)
	int EnemyKillMax;
	bool QuestClear;
protected:

	UFUNCTION()
		void ReceiveDamageEvent(AActor* DamageActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);
public:
	virtual void ActivateObjective();
	virtual FName GetObjectiveName();
	virtual bool GetIsComplete() { return QuestClear; }
	void PrevInit(class AMultiPlayPlayerController* Controller, FName ObjectName, int iEnemyKillMax, class AFieldManager* Manager);
};
