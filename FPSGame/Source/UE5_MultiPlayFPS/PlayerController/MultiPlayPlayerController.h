// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "MultiPlayPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API AMultiPlayPlayerController : 
	public APlayerController,
	public IGenericTeamAgentInterface
{
	GENERATED_BODY()
public:
	AMultiPlayPlayerController();
	
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	virtual void BeginPlay() override;
	void SetHUDTime();
	void PollInit();
	//서버와 클라이언트 동기화

	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);

	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	float ClientServerDelta;
	UPROPERTY(EditAnywhere, Category=Time)
	float TimeSyncFrequency;
	float TimeSyncRunningTime;
	void CheckTimeSync(float DeltaTime);
	
	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	//진행중인 게임 중간접속
	UFUNCTION(Client, Reliable)
	void ClientJoinMidgame(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime);


public:

private:
	UPROPERTY()
	class AMultiPlayHUD* MultiPlayHUD;

	class AMultiPlayGameMode* MultiPlayGameMode;
	float LevelStartingTime;
	float MatchTime;
	float WarmupTime;
	float CooldownTime;
	int CountDownInt;

	UPROPERTY(ReplicatedUsing=OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class UCharacterOverlay* CharacterOverlay;
	bool bInitializeCharacterOverlay;

	float HUDHealth;
	bool bInitHealth;
	float HUDMaxHealth;
	float HUDShield;
	bool bInitShield;
	float HUDMaxShield;
	float HUDScore;
	bool bInitScore;
	int32 HUDDefeats;
	bool bInitDefeats;
	float HUDGrenades;
	bool bInitGrenades;

	FGenericTeamId TeamId;
public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDShield(float Shield, float MaxShield);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDMatchCountdown(float CountDown);
	void SetHUDAnnouncementCountdown(float Countdown);
	void SetHUDGrenades(int Grenades);
	void SetHUDInventory(class UCombatComponent* Combat);
	void SetHUDSkill(class UAbilityComponent* Ability);
	void SetHUDBossHealthBar(float BossHealth, float BossHelathMax);

	virtual float GetServerTime();
	virtual void ReceivedPlayer() override;
	void OnMatchStateSet(FName State);
	void HandleMatchHasStarted();
	void HandleCooldown();
	void ShowCross();
	void ShopOpen();
	void ShopClose();
	virtual FGenericTeamId GetGenericTeamId() const override;
	class UCharacterOverlay* GetCharacterOverlay() { return CharacterOverlay; }
};
