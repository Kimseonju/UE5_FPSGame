// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillSlot.generated.h"

/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API USkillSlot : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(meta = (BindWidget))
	class UImage* SkillImage;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* CooldownBar;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CooldownText;
};
