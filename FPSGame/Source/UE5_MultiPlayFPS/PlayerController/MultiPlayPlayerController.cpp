// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiPlayPlayerController.h"
#include "UE5_MultiPlayFPS/Widget/HUD/MultiPlayHUD.h"
#include "UE5_MultiPlayFPS/Widget/HUD/CharacterOverlay.h"
#include "UE5_MultiPlayFPS/Widget/HUD/Announcement.h"
#include "UE5_MultiPlayFPS/Widget/HUD/InventoryBar.h"
#include "UE5_MultiPlayFPS/Widget/HUD/SkillBar.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "UE5_MultiPlayFPS/Character/MultiPlayCharacter.h"
#include "Net/UnrealNetwork.h"
#include "UE5_MultiPlayFPS/GameMode/MultiPlayGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "UE5_MultiPlayFPS/MultiPlayComponents/CombatComponent.h"
#include "UE5_MultiPlayFPS/GameState/MultiPlayGameState.h"
#include "UE5_MultiPlayFPS/PlayerState/MultiPlayPlayerState.h"
AMultiPlayPlayerController::AMultiPlayPlayerController()
{
	MatchTime = 0.f;
	ClientServerDelta = 0.f;
	TimeSyncFrequency = 5.f;
	TimeSyncRunningTime = 0.f;
	bInitializeCharacterOverlay = false;
	WarmupTime = 0.f;
	LevelStartingTime = 0.f;
	CooldownTime = 0.f;
	bInitHealth = false;
	bInitShield = false;
	bInitScore = false;
	bInitDefeats = false;
	bInitGrenades = false;
}

void AMultiPlayPlayerController::BeginPlay()
{
	Super::BeginPlay();
	MultiPlayHUD = Cast<AMultiPlayHUD>(GetHUD());
	ServerCheckMatchState();
}

void AMultiPlayPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetHUDTime();
	CheckTimeSync(DeltaTime);
	PollInit();
}


void AMultiPlayPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMultiPlayPlayerController, MatchState);
}

void AMultiPlayPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime >= TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime -= TimeSyncFrequency;
	}
}



void AMultiPlayPlayerController::ServerCheckMatchState_Implementation()
{
	AMultiPlayGameMode* GameMode = Cast<AMultiPlayGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		CooldownTime = GameMode->CooldownTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		MatchState = GameMode->GetMatchState();
		ClientJoinMidgame(MatchState, WarmupTime, MatchTime, CooldownTime, LevelStartingTime);
	}
}

void AMultiPlayPlayerController::ClientJoinMidgame_Implementation(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime)
{
	MatchState = StateOfMatch;
	WarmupTime = Warmup;
	MatchTime = Match;
	LevelStartingTime = StartingTime;
	CooldownTime = Cooldown;
	OnMatchStateSet(MatchState);

	if (MultiPlayHUD && MatchState == MatchState::WaitingToStart)
	{
		MultiPlayHUD->AddAnnouncement();
	}
}

void AMultiPlayPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AMultiPlayCharacter* PlayCharacter = Cast<AMultiPlayCharacter>(InPawn);
	if (PlayCharacter)
	{
		SetHUDHealth(PlayCharacter->GetHealth(), PlayCharacter->GetMaxHealth());
	}
}

void AMultiPlayPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	MultiPlayHUD = MultiPlayHUD == nullptr ? Cast<AMultiPlayHUD>(GetHUD()) : MultiPlayHUD;

	bool bHUDValid = MultiPlayHUD && 
		MultiPlayHUD->CharacterOverlay &&
		MultiPlayHUD->CharacterOverlay->HealthBar && 
		MultiPlayHUD->CharacterOverlay->HealthText;
	if (bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		MultiPlayHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		MultiPlayHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
	else
	{
		bInitHealth = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
	}

}

void AMultiPlayPlayerController::SetHUDShield(float Shield, float MaxShield)
{
	MultiPlayHUD = MultiPlayHUD == nullptr ? Cast<AMultiPlayHUD>(GetHUD()) : MultiPlayHUD;

	bool bHUDValid = MultiPlayHUD &&
		MultiPlayHUD->CharacterOverlay &&
		MultiPlayHUD->CharacterOverlay->ShieldBar &&
		MultiPlayHUD->CharacterOverlay->ShieldText;
	if (bHUDValid)
	{
		const float ShieldPercent = Shield / MaxShield;
		MultiPlayHUD->CharacterOverlay->ShieldBar->SetPercent(ShieldPercent);
		FString ShieldText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Shield), FMath::CeilToInt(MaxShield));
		MultiPlayHUD->CharacterOverlay->ShieldText->SetText(FText::FromString(ShieldText));
	}
	else
	{
		bInitShield = true;
		HUDShield = Shield;
		HUDMaxShield = MaxShield;
	}
}

void AMultiPlayPlayerController::SetHUDScore(float Score)
{
	MultiPlayHUD = MultiPlayHUD == nullptr ? Cast<AMultiPlayHUD>(GetHUD()) : MultiPlayHUD;

	bool bHUDValid = MultiPlayHUD &&
		MultiPlayHUD->CharacterOverlay &&
		MultiPlayHUD->CharacterOverlay->ScoreAmountText;
	if (bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		MultiPlayHUD->CharacterOverlay->ScoreAmountText->SetText(FText::FromString(ScoreText));
	}
	else
	{
		bInitScore = true;
		HUDScore = Score;
	}
}

void AMultiPlayPlayerController::SetHUDDefeats(int32 Defeats)
{
	MultiPlayHUD = MultiPlayHUD == nullptr ? Cast<AMultiPlayHUD>(GetHUD()) : MultiPlayHUD;
	bool bHUDValid = MultiPlayHUD &&
		MultiPlayHUD->CharacterOverlay &&
		MultiPlayHUD->CharacterOverlay->DefeatsAmountText;
	if (bHUDValid)
	{
		FString DefeatsText = FString::Printf(TEXT("%d"), Defeats);
		MultiPlayHUD->CharacterOverlay->DefeatsAmountText->SetText(FText::FromString(DefeatsText));
	}
	else
	{
		bInitDefeats = true;
		HUDDefeats = Defeats;
	}
}

void AMultiPlayPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	MultiPlayHUD = MultiPlayHUD == nullptr ? Cast<AMultiPlayHUD>(GetHUD()) : MultiPlayHUD;
	bool bHUDValid = MultiPlayHUD &&
		MultiPlayHUD->CharacterOverlay &&
		MultiPlayHUD->CharacterOverlay->AmmoAmountText;
	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		MultiPlayHUD->CharacterOverlay->AmmoAmountText->SetText(FText::FromString(AmmoText));
	}
}

void AMultiPlayPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	MultiPlayHUD = MultiPlayHUD == nullptr ? Cast<AMultiPlayHUD>(GetHUD()) : MultiPlayHUD;
	bool bHUDValid = MultiPlayHUD &&
		MultiPlayHUD->CharacterOverlay &&
		MultiPlayHUD->CharacterOverlay->CarriedAmountText;
	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		MultiPlayHUD->CharacterOverlay->CarriedAmountText->SetText(FText::FromString(AmmoText));
	}
}

void AMultiPlayPlayerController::SetHUDMatchCountdown(float CountDown)
{
	MultiPlayHUD = MultiPlayHUD == nullptr ? Cast<AMultiPlayHUD>(GetHUD()) : MultiPlayHUD;
	bool bHUDValid = MultiPlayHUD &&
		MultiPlayHUD->CharacterOverlay &&
		MultiPlayHUD->CharacterOverlay->MatchCountdownText;
	if (bHUDValid)
	{
		if (CountDown < 0.f)
		{
			MultiPlayHUD->CharacterOverlay->MatchCountdownText->SetText(FText());
			return;
		}
		int Minute = FMath::FloorToInt(CountDown / 60.f);
		int Second = CountDown - Minute * 60;
		
		
		FString CountDownText = FString::Printf(TEXT("%02d:%02d"), Minute, Second);
		MultiPlayHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountDownText));
	}
}


void AMultiPlayPlayerController::SetHUDAnnouncementCountdown(float Countdown)
{
	MultiPlayHUD = MultiPlayHUD == nullptr ? Cast<AMultiPlayHUD>(GetHUD()) : MultiPlayHUD;
	bool bHUDValid = MultiPlayHUD &&
		MultiPlayHUD->Announcement &&
		MultiPlayHUD->Announcement->WarmupTime;
	if (bHUDValid)
	{
		if (Countdown < 0.f)
		{
			MultiPlayHUD->Announcement->WarmupTime->SetText(FText());
			return;
		}
		int Minute = FMath::FloorToInt(Countdown / 60.f);
		int Second = Countdown - Minute * 60;


		FString CountDownText = FString::Printf(TEXT("%02d:%02d"), Minute, Second);
		MultiPlayHUD->Announcement->WarmupTime->SetText(FText::FromString(CountDownText));
	}
}

void AMultiPlayPlayerController::SetHUDGrenades(int Grenades)
{
	MultiPlayHUD = MultiPlayHUD == nullptr ? Cast<AMultiPlayHUD>(GetHUD()) : MultiPlayHUD;
	bool bHUDValid = MultiPlayHUD &&
		MultiPlayHUD->CharacterOverlay &&
		MultiPlayHUD->CharacterOverlay->GrenadeText;
	if (bHUDValid)
	{
		FString GrenadeText = FString::Printf(TEXT("%d"), Grenades);
		MultiPlayHUD->CharacterOverlay->GrenadeText->SetText(FText::FromString(GrenadeText));
	}
	else
	{
		HUDGrenades = Grenades;
	}
}

void AMultiPlayPlayerController::SetHUDInventory(UCombatComponent* Combat)
{
	MultiPlayHUD = MultiPlayHUD == nullptr ? Cast<AMultiPlayHUD>(GetHUD()) : MultiPlayHUD;
	bool bHUDValid = MultiPlayHUD &&
		MultiPlayHUD->CharacterOverlay &&
		MultiPlayHUD->CharacterOverlay->InventoryBar;
	if (bHUDValid)
	{
		MultiPlayHUD->CharacterOverlay->InventoryBar->UpdateSlot(Combat);
	}
}

void AMultiPlayPlayerController::SetHUDSkill(class UAbilityComponent* Ability)
{
	MultiPlayHUD = MultiPlayHUD == nullptr ? Cast<AMultiPlayHUD>(GetHUD()) : MultiPlayHUD;
	bool bHUDValid = MultiPlayHUD &&
		MultiPlayHUD->CharacterOverlay &&
		MultiPlayHUD->CharacterOverlay->SkillBar;
	if (bHUDValid)
	{
		MultiPlayHUD->CharacterOverlay->SkillBar->UpdateSlot(Ability);
	}
}

void AMultiPlayPlayerController::SetHUDBossHealthBar(float BossHealth, float BossHelathMax)
{
	MultiPlayHUD = MultiPlayHUD == nullptr ? Cast<AMultiPlayHUD>(GetHUD()) : MultiPlayHUD;
	bool bHUDValid = MultiPlayHUD &&
		MultiPlayHUD->CharacterOverlay &&
		MultiPlayHUD->CharacterOverlay->BossHealthBar;
	if (bHUDValid)
	{
		const float HealthPercent = BossHealth / BossHelathMax;
		if (HealthPercent > 0.f)
		{
			MultiPlayHUD->CharacterOverlay->BossHealthBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			MultiPlayHUD->CharacterOverlay->BossHealthBar->SetPercent(HealthPercent);
		}
		else
		{
			MultiPlayHUD->CharacterOverlay->BossHealthBar->SetVisibility(ESlateVisibility::Hidden);
		}

	}
}

void AMultiPlayPlayerController::SetHUDTime()
{
	float TimeLeft = 0.f;
	if (MatchState == MatchState::WaitingToStart) TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::InProgress) TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	else if(MatchState==MatchState::Cooldown) TimeLeft = CooldownTime + WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	int SecondLeft = FMath::CeilToInt(TimeLeft);


	if (HasAuthority())
	{
		MultiPlayGameMode = MultiPlayGameMode == nullptr ? Cast<AMultiPlayGameMode>(UGameplayStatics::GetGameMode(this)) : MultiPlayGameMode;
		if (MultiPlayGameMode)
		{
			SecondLeft = FMath::CeilToInt(MultiPlayGameMode->GetCountdownTime() + LevelStartingTime);
		}
	}

	if (CountDownInt != SecondLeft)
	{
		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}
		if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);
		}
		SetHUDMatchCountdown(SecondLeft);
	}
	CountDownInt = SecondLeft;
}


void AMultiPlayPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (MultiPlayHUD && MultiPlayHUD->CharacterOverlay)
		{
			CharacterOverlay = MultiPlayHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				if(bInitHealth)  SetHUDHealth(HUDHealth, HUDMaxHealth);
				if (bInitShield) SetHUDShield(HUDShield, HUDMaxShield);
				if (bInitScore)  SetHUDScore(HUDScore);
				if (bInitDefeats)SetHUDDefeats(HUDDefeats);
				AMultiPlayCharacter* MultiPlayCharacter = Cast<AMultiPlayCharacter>(GetPawn());
				if (MultiPlayCharacter && MultiPlayCharacter->GetCombat())
				{
					if(bInitGrenades) SetHUDGrenades(MultiPlayCharacter->GetCombat()->GetGrands());
				}
			}
		}
	}
}

void AMultiPlayPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void AMultiPlayPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedClientRequest)
{
	//왕복시간
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest; 
	//클라 + 서버->클라로 보내는데 걸리는 시간만 계산
	float CurrentServerTime = TimeServerReceivedClientRequest + (0.5f * RoundTripTime);
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

float AMultiPlayPlayerController::GetServerTime()
{
	return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void AMultiPlayPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void AMultiPlayPlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void AMultiPlayPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void AMultiPlayPlayerController::HandleMatchHasStarted()
{
	MultiPlayHUD = MultiPlayHUD == nullptr ? Cast<AMultiPlayHUD>(GetHUD()) : MultiPlayHUD;
	if (MultiPlayHUD)
	{
		if (MultiPlayHUD->CharacterOverlay == nullptr)MultiPlayHUD->AddCharacterOverlay();
		if (MultiPlayHUD->ShopOverlay == nullptr)MultiPlayHUD->AddShopOverlay();
		if (MultiPlayHUD->ShopOverlay) ShopClose();
		if (MultiPlayHUD->Announcement)
		{
			MultiPlayHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void AMultiPlayPlayerController::HandleCooldown()
{
	MultiPlayHUD = MultiPlayHUD == nullptr ? Cast<AMultiPlayHUD>(GetHUD()) : MultiPlayHUD;
	if (MultiPlayHUD)
	{
		MultiPlayHUD->CharacterOverlay->RemoveFromParent();
		bool bHUDValid = MultiPlayHUD->Announcement && 
			MultiPlayHUD->Announcement->AnnouncementText && 
			MultiPlayHUD->Announcement->InfoText;
		if (bHUDValid)
		{
			MultiPlayHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
			FString AnnouncementText("New Match Starts In:");
			MultiPlayHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));
			AMultiPlayGameState* PlayGameState = Cast<AMultiPlayGameState>(UGameplayStatics::GetGameState(this));
			AMultiPlayPlayerState* PlayPlayerState = GetPlayerState<AMultiPlayPlayerState>();
			if (PlayGameState && PlayPlayerState)
			{
				TArray<AMultiPlayPlayerState*>TopPlayers = PlayGameState->TopScoringPlayers;
				FString InfoText;
				if (TopPlayers.Num() == 0)
				{
					InfoText = FString("No Kill leader");
				}
				else if (TopPlayers.Num() == 1 && TopPlayers[0] == PlayPlayerState)
				{
					InfoText = FString("Kill leader");
				}
				else if (TopPlayers.Num() == 1)
				{
					InfoText = FString::Printf(TEXT("Kill leader: \n%s"), *TopPlayers[0]->GetPlayerName());
				}
				else if (TopPlayers.Num() > 1)
				{
					InfoText = FString("Kill leaders\n");
					for (auto TiedPlayer : TopPlayers)
					{
						InfoText.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName()));
					}
				}
				MultiPlayHUD->Announcement->InfoText->SetText(FText::FromString(InfoText));
			}
		}
	}
	AMultiPlayCharacter* PlayCharacter = Cast<AMultiPlayCharacter>(GetPawn());
	if (PlayCharacter && PlayCharacter->GetCombat())
	{
		PlayCharacter->bDisableGameplay = true;
		PlayCharacter->GetCombat()->FireBtnPressed(false);
	}
}

void AMultiPlayPlayerController::ShowCross()
{
	MultiPlayHUD = MultiPlayHUD == nullptr ? Cast<AMultiPlayHUD>(GetHUD()) : MultiPlayHUD;
	bool bHUDValid = MultiPlayHUD &&
		MultiPlayHUD->CharacterOverlay;

	if (bHUDValid)
	{
		MultiPlayHUD->CharacterOverlay->ShowCross();
	}
}

void AMultiPlayPlayerController::ShopOpen()
{
	MultiPlayHUD = MultiPlayHUD == nullptr ? Cast<AMultiPlayHUD>(GetHUD()) : MultiPlayHUD;
	bool bHUDValid = MultiPlayHUD &&
		MultiPlayHUD->CharacterOverlay &&
		MultiPlayHUD->ShopOverlay;

	if (bHUDValid)
	{
		MultiPlayHUD->CharacterOverlay->SetVisibility(ESlateVisibility::Hidden);
		MultiPlayHUD->ShopOverlay->SetVisibility(ESlateVisibility::Visible);
		FInputModeGameAndUI mode;
		bShowMouseCursor = true;
		SetInputMode(mode);
	}
}

void AMultiPlayPlayerController::ShopClose()
{
	MultiPlayHUD = MultiPlayHUD == nullptr ? Cast<AMultiPlayHUD>(GetHUD()) : MultiPlayHUD;
	bool bHUDValid = MultiPlayHUD &&
		MultiPlayHUD->CharacterOverlay &&
		MultiPlayHUD->ShopOverlay;

	if (bHUDValid)
	{
		MultiPlayHUD->CharacterOverlay->SetVisibility(ESlateVisibility::Visible);
		MultiPlayHUD->ShopOverlay->SetVisibility(ESlateVisibility::Hidden);
		FInputModeGameOnly mode;
		bShowMouseCursor = false;
		SetInputMode(mode);
	}
}

FGenericTeamId AMultiPlayPlayerController::GetGenericTeamId() const
{
	return TeamId;
}
