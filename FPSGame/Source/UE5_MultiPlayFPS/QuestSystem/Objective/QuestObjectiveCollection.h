// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestObjective.h"
#include "QuestObjectiveCollection.generated.h"

/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API UQuestObjectiveCollection : public UQuestObjective
{
	GENERATED_BODY()

private:
	bool bOrderRequired;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TArray< UQuestObjective*> Objectives;
	class ANPC* QuestNPC;
protected:
	UQuestObjective* FindNextInCompleteObject();

	UFUNCTION()
		void OnCompletedBind(UQuestObjective* Objective);

public:
	void SetNPC(class ANPC* NPC) { QuestNPC = NPC; }
	void AddObjective(UQuestObjective* Objective) { Objectives.Add(Objective); }
	virtual void ActivateObjective();
	void PrevInit(class AMultiPlayPlayerController* Controller, bool OrderRequired, FName ObjectName);
};
