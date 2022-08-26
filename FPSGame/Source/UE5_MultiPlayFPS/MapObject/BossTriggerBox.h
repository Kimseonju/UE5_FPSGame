// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "BossTriggerBox.generated.h"

/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API ABossTriggerBox : public ATriggerBox
{
	GENERATED_BODY()
public:
	ABossTriggerBox();
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class ULevelSequence* m_LvSeq;

	class ULevelSequencePlayer* m_LvSeqPlayer;
	class ALevelSequenceActor* m_LvSeqActor;

protected:
	virtual void BeginPlay() override;
protected:
	UFUNCTION()
	void BeginTrigger(AActor* _TriggerActor, AActor* _OtherActor);

	UFUNCTION()
	void EndTrigger(AActor* _TriggerActor, AActor* _OtherActor);
};
