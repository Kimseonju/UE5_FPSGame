// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MultiPlayPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API AMultiPlayPlayerState : public APlayerState
{
	GENERATED_BODY()

public:		

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//서버에 실행되는 함수
	void AddToScore(float ScoreAmount);
	void AddToDefeats(int32 DefeatsAmount);
	virtual void OnRep_Score() override;

	UFUNCTION()
	void OnRep_Defeats();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FTransform SaveTransform;
	bool bMultiPlay;
private:
	
	UPROPERTY()
	class AMultiPlayCharacter* Character;
	UPROPERTY()
	class AMultiPlayPlayerController* Controller;

	UPROPERTY(ReplicatedUsing=OnRep_Defeats)
	int32 Defeats;
public:
	void Save(FTransform Transform);
	bool IsMultiPlay() { return bMultiPlay; }
	FTransform GetSaveTransform() { return SaveTransform; }
};
