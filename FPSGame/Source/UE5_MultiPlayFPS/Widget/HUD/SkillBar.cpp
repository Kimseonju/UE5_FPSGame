// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillBar.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "UE5_MultiPlayFPS/MultiPlayComponents/AbilityComponent.h"
#include "UE5_MultiPlayFPS/Widget/HUD/SkillSlot.h"
void USkillBar::UpdateSlot(class UAbilityComponent* Ability)
{
	if (Ability)
	{
		float ScanTimer = Ability->GetScanTimer();
		if (ScanTimer < 0.f)
			ScanTimer = 0.f;
		float ScanTimerMax = Ability->GetScanTimerMax();
		float ScanPercent = ScanTimer / ScanTimerMax;

		Skill1->CooldownBar->SetPercent(ScanPercent);
		int iScanTimer = (int)ScanTimer;
		FString CooldownText = FString::Printf(TEXT("%d"), iScanTimer);
		Skill1->CooldownText->SetText(FText::FromString(CooldownText));


		float PortalTimer = Ability->GetCreatePortalTimer();
		if (PortalTimer < 0.f)
			PortalTimer = 0.f;
		float PortalTimerMax = Ability->GetCreatePortalTimerMax();
		float PortalPercent = PortalTimer / PortalTimerMax;

		Skill2->CooldownBar->SetPercent(PortalPercent);
		int iPortalTimer = (int)PortalTimer;
		CooldownText = FString::Printf(TEXT("%d"), iPortalTimer);
		Skill2->CooldownText->SetText(FText::FromString(CooldownText));
		if (Ability->GetPortalImage())
		{
			Skill2->SkillImage->SetBrushFromTexture(Ability->GetPortalImage());
		}
		
	}
}
