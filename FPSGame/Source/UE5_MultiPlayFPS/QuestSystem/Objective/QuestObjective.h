// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "QuestObjective.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCompletedDelegate, UQuestObjective*, Objective);
/**
 * 
 */
UCLASS(Blueprintable)
class UE5_MULTIPLAYFPS_API UQuestObjective : public UObject
{
	GENERATED_BODY()
	

protected:
	FName Name;
	bool bActive;
	class AMultiPlayPlayerController* PlayerController;

public:
	UFUNCTION(BlueprintCallable)
	virtual FName GetObjectiveName() { return Name; }
	UFUNCTION(BlueprintCallable)
	virtual bool GetIsComplete() { return true; }
	UFUNCTION(BlueprintCallable)
	virtual bool GetIsActive()const { return bActive; }
	class AMultiPlayPlayerController* GetPlayerController() { return PlayerController; }
	void SetPlayerController(class AMultiPlayPlayerController* Controller) { PlayerController = Controller; }
	virtual void ActivateObjective();
	UPROPERTY(BlueprintAssignable)
	FCompletedDelegate OnCompletedDelegate;
};
