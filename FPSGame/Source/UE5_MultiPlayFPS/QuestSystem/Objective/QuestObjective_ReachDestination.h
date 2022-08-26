// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestObjective.h"
#include "QuestObjective_ReachDestination.generated.h"

/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API UQuestObjective_ReachDestination : public UQuestObjective
{
	GENERATED_BODY()
	bool QuestClear;
	UPROPERTY(EditAnywhere)
	class ATriggerBox* TriggerBox;
protected:

	UFUNCTION()
		void BeginTrigger(AActor* _TriggerActor, AActor* _OtherActor);

public:
	virtual void ActivateObjective();
	virtual bool GetIsComplete() { return QuestClear; }

	void PrevInit(class AMultiPlayPlayerController* Controller, FName ObjectName, class ATriggerBox* TriggerBox_);
};
