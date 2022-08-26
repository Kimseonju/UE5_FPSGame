// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Quest.h"
#include "QuestBossEnemy.generated.h"

/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API AQuestBossEnemy : public AQuest
{
	GENERATED_BODY()

protected:

	int EnemyKill;
protected:
	virtual class UQuestObjectiveCollection* ConstructRootObjectiveCollection();
public:
	virtual void PopulateObjective(class UQuestObjectiveCollection* ObjectiveCollection);
	UFUNCTION(BlueprintCallable)
		void PrevInit(int EnemyKillMax);
};
