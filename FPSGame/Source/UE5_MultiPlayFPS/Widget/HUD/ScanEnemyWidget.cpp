// Fill out your copyright notice in the Description page of Project Settings.


#include "ScanEnemyWidget.h"

#include "Components/Image.h"

FVector2D UScanEnemyWidget::GetWidgetSize() const
{
	if (Image)
	{
		return Image->GetDesiredSize();
	}
	else
	{
		return FVector2D();
	}
}
