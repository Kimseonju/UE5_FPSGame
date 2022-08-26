// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScanEnemyWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API UScanEnemyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UImage* Image;

	FVector2D GetWidgetSize() const;
};
