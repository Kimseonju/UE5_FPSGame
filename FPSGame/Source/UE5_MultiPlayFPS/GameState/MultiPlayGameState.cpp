// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiPlayGameState.h"
#include "Net/UnrealNetwork.h"
#include "UE5_MultiPlayFPS/PlayerState/MultiPlayPlayerState.h"
AMultiPlayGameState::AMultiPlayGameState()
{
	TopScore = 0.f;
}

void AMultiPlayGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMultiPlayGameState, TopScoringPlayers);
}

void AMultiPlayGameState::UpdateTopScore(AMultiPlayPlayerState* ScoringPlayer)
{
	if (TopScoringPlayers.Num() == 0)
	{
		TopScoringPlayers.Add(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
	else if (ScoringPlayer->GetScore() == TopScore)
	{
		TopScoringPlayers.AddUnique(ScoringPlayer);
	}
	else if (ScoringPlayer->GetScore() > TopScore)
	{
		TopScoringPlayers.Empty();
		TopScoringPlayers.AddUnique(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
}
