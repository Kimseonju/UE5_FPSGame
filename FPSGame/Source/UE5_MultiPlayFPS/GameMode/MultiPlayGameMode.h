// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MultiPlayGameMode.generated.h"

namespace MatchState
{
	extern UE5_MULTIPLAYFPS_API const FName Cooldown; // 게임이 끝나고 승자를 표시할 때 표시
}

/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API AMultiPlayGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AMultiPlayGameMode();
	virtual void Tick(float DeltaTime) override;
	virtual void PlayerEliminated(class AMultiPlayCharacter* ElimCharacter, class AMultiPlayPlayerController* VictimController,
		class AMultiPlayPlayerController* AttackerController);

	virtual void PlayerKillEnemy(int EnemyScore, class AMultiPlayPlayerController* AttackerController);
	virtual void RequestRespawn(class ACharacter* ElimmedCharacter, AController* ElimmedController);

	UPROPERTY(EditDefaultsOnly)
	float WarmupTime;

	UPROPERTY(EditDefaultsOnly)
	float MatchTime;

	UPROPERTY(EditDefaultsOnly)
	float CooldownTime;
	float LevelStartingTime = 0.f;
protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;
private:
	float CountdownTime = 0.f;

public:
	float GetCountdownTime() const { return CountdownTime; }
};
