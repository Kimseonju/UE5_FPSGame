// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageIndicatorWidget.generated.h"
/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API UDamageIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	float LifeTime;
public:

	UPROPERTY(meta = (BindWidget))
	class UBorder* HitWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UMaterialInstanceDynamic* DynMaterial;

	UFUNCTION(BlueprintImplementableEvent)
	void SetInit(class ACharacter* HitCharacter, FVector HitLocation);
};
