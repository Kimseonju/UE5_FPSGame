// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageIndicatorWidget.h"
#include "Components/Border.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
void UDamageIndicatorWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (HitWidget)
	{
		DynMaterial =HitWidget->GetDynamicMaterial();
	}
	LifeTime = 2.f;

}

void UDamageIndicatorWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	LifeTime -= InDeltaTime;
	if (LifeTime <= 0.f)
	{
		RemoveFromParent();
	}
}
