// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillBar.generated.h"

class USkillSlot;
/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API USkillBar : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(meta = (BindWidget))
	USkillSlot* Skill1;

	UPROPERTY(meta = (BindWidget))
	USkillSlot* Skill2;

	void UpdateSlot(class UAbilityComponent* Ability);
};
