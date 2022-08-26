// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileThrow.generated.h"

/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API AProjectileThrow : public AProjectile
{
	GENERATED_BODY()
public:
	AProjectileThrow();
	virtual void Tick(float DeltaTime) override;
	virtual void ObjectPoolInUse(bool Enable);
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
		virtual void OnBouse(const FHitResult& ImpactResult, const FVector& ImpactVelocity);
	virtual void Destroyed() override;

	UPROPERTY(EditAnywhere)
		class USoundCue* BouseSound;
};
