// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HitScanWeapon.h"
#include "ShotGun.generated.h"

/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API AShotGun : public AHitScanWeapon
{
	GENERATED_BODY()

public:
	AShotGun();
	virtual void Fire(const FVector& HitTarget) override;
private:
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	int FirePellets;
};
