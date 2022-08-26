// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MultiPlayGameState.generated.h"

class AMultiPlayPlayerState;
UCLASS()
class UE5_MULTIPLAYFPS_API AMultiPlayGameState : public AGameState
{
	GENERATED_BODY()
public:
	AMultiPlayGameState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateTopScore(AMultiPlayPlayerState* ScoringPlayer);
	UPROPERTY(Replicated)
	TArray<AMultiPlayPlayerState*> TopScoringPlayers;

private:
	float TopScore;
};
