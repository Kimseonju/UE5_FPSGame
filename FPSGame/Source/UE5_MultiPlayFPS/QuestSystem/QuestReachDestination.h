// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Quest.h"
#include "QuestReachDestination.generated.h"

/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API AQuestReachDestination : public AQuest
{
	GENERATED_BODY()

protected:
	int EnemyKill;
	UPROPERTY(EditAnywhere)
	class ATriggerBox* TriggerBox1;
	UPROPERTY(EditAnywhere)
	class ATriggerBox* TriggerBox2;
	UPROPERTY(EditAnywhere)
	class ATriggerBox* TriggerBox3;

	UPROPERTY(EditAnywhere)
	FName TriggerName1;
	UPROPERTY(EditAnywhere)
	FName TriggerName2;
	UPROPERTY(EditAnywhere)
	FName TriggerName3;
protected:
	virtual class UQuestObjectiveCollection* ConstructRootObjectiveCollection();
public:
	virtual void PopulateObjective(class UQuestObjectiveCollection* ObjectiveCollection);
	UFUNCTION(BlueprintCallable)
	void PrevInit(class ATriggerBox* Box1, class ATriggerBox* Box2, class ATriggerBox* Box3, FName Name1, FName Name2, FName Name3);
};
