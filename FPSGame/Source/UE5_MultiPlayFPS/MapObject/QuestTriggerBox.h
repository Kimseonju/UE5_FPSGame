// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "QuestTriggerBox.generated.h"

/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API AQuestTriggerBox : public ATriggerBox
{
	GENERATED_BODY()

public:
	AQuestTriggerBox();
private:
	UPROPERTY(VisibleAnywhere)
		class UParticleSystemComponent* ParticleComponent;
	class USceneComponent* RootComp;

protected:
	virtual void BeginPlay() override;
public:
	UFUNCTION(BlueprintCallable)
	void QuestOn();
	UFUNCTION(BlueprintCallable)
	void QuestOff();
	UFUNCTION()
	void BeginTrigger(AActor* _TriggerActor, AActor* _OtherActor);

	UFUNCTION()
	void EndTrigger(AActor* _TriggerActor, AActor* _OtherActor);
};
