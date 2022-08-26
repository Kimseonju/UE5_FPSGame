// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyHitNumberWidget.h"
#include "COmponents/TextBlock.h"

void UEnemyHitNumberWidget::SetHitNumber(int Number, bool Headshot)
{
	HitNumber->SetText(FText::FromString(FString::FromInt(Number)));

}
