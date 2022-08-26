// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

class UProgressBar;
class UTextBlock;
UCLASS()
class UE5_MULTIPLAYFPS_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta=(BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
		UProgressBar* BossHealthBar;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthText;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ShieldBar;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ShieldText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreAmountText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DefeatsAmountText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AmmoAmountText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CarriedAmountText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MatchCountdownText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GrenadeText;

	UPROPERTY(meta = (BindWidget))
	class UInventoryBar* InventoryBar;

	UPROPERTY(meta = (BindWidget))
	class USkillBar* SkillBar;

	UFUNCTION(BlueprintImplementableEvent)
	void ShowCross();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void QuestTextEvent(const FText& Text);
};
