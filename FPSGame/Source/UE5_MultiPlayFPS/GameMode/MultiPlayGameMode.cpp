// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiPlayGameMode.h"
#include "UE5_MultiPlayFPS/Character/MultiPlayCharacter.h"
#include "UE5_MultiPlayFPS/PlayerController/MultiPlayPlayerController.h"
#include "Kismet//GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "UE5_MultiPlayFPS/PlayerState/MultiPlayPlayerState.h"
#include "UE5_MultiPlayFPS/GameState/MultiPlayGameState.h"
namespace MatchState
{
	const FName Cooldown = FName(TEXT("Cooldown"));
}

AMultiPlayGameMode::AMultiPlayGameMode()
{
	bDelayedStart = true;
	WarmupTime = 10.f;
	MatchTime = 120.f;
	CooldownTime = 10.f;
}

void AMultiPlayGameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void AMultiPlayGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		CountdownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		CountdownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			RestartGame();
		}
	}
}

void AMultiPlayGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator iter = GetWorld()->GetPlayerControllerIterator(); iter; ++iter)
	{
		AMultiPlayPlayerController* PlayerController = Cast<AMultiPlayPlayerController>(*iter);
		if (PlayerController)
		{
			PlayerController->OnMatchStateSet(MatchState);
		}
	}
}



void AMultiPlayGameMode::PlayerEliminated(class AMultiPlayCharacter* ElimCharacter, class AMultiPlayPlayerController* VictimController, class AMultiPlayPlayerController* AttackerController)
{
	AMultiPlayPlayerState* AttackerPlayerState = AttackerController ? Cast<AMultiPlayPlayerState>(AttackerController->PlayerState) : nullptr;
	AMultiPlayPlayerState* VictimPlayerState = VictimController ? Cast<AMultiPlayPlayerState>(VictimController->PlayerState) : nullptr;
	AMultiPlayGameState* PlayGameState = GetGameState<AMultiPlayGameState>();
	//ÀÚ»ì½Ã X
	if (AttackerPlayerState && AttackerPlayerState!=VictimPlayerState && PlayGameState)
	{
		AttackerPlayerState->AddToScore(1.f);
		PlayGameState->UpdateTopScore(AttackerPlayerState);

	}
	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}

	if (ElimCharacter)
	{
		ElimCharacter->Elim();
	}
}

void AMultiPlayGameMode::PlayerKillEnemy(int EnemyScore, class AMultiPlayPlayerController* AttackerController)
{
	AMultiPlayPlayerState* AttackerPlayerState = AttackerController ? Cast<AMultiPlayPlayerState>(AttackerController->PlayerState) : nullptr;

	AMultiPlayGameState* PlayGameState = GetGameState<AMultiPlayGameState>();

	if (AttackerPlayerState && PlayGameState)
	{
		AttackerPlayerState->AddToScore(EnemyScore);
		PlayGameState->UpdateTopScore(AttackerPlayerState);
	}
}

void AMultiPlayGameMode::RequestRespawn(class ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}

	if (ElimmedController)
	{
		AMultiPlayPlayerState* PlayState=ElimmedController->GetPlayerState<AMultiPlayPlayerState>();

		TArray<AActor*> PlayerStartActors;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStartActors);
		if (PlayState)
		{
			if (PlayState->IsMultiPlay())
			{
				int32 Selection = FMath::RandRange(0, PlayerStartActors.Num() - 1);
				RestartPlayerAtPlayerStart(ElimmedController, PlayerStartActors[Selection]);
			}
			else
			{
				if (PlayState->GetSaveTransform().GetLocation() == FVector(0.f, 0.f, 0.f))
				{

					int32 Selection = FMath::RandRange(0, PlayerStartActors.Num() - 1);
					RestartPlayerAtPlayerStart(ElimmedController, PlayerStartActors[Selection]);
				}
				else
				{
					RestartPlayerAtTransform(ElimmedController, PlayState->GetSaveTransform());
				}
			}
		}
		else
		{
			int32 Selection = FMath::RandRange(0, PlayerStartActors.Num() - 1);
			RestartPlayerAtPlayerStart(ElimmedController, PlayerStartActors[Selection]);
		}
	}
}
