// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "HitScanWeapon.generated.h"

class UParticleSystem;

UCLASS()
class UE5_MULTIPLAYFPS_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()
public:
	AHitScanWeapon();
	virtual void Fire(const FVector& HitTarget) override;
protected:
	UPROPERTY(EditAnywhere)
	float Damage;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticle;

	UPROPERTY(EditAnywhere)
	UParticleSystem* BeamParticle;

	UPROPERTY(EditAnywhere)
	UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere)
	USoundCue* FireSound;

	UPROPERTY(EditAnywhere)
	USoundCue* HitSound;

	UPROPERTY(EditAnywhere, Category="Weapon Scatter")
	float DistanceToSphere;
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	float SphereRadius;
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	bool bUseScatter;
	FVector TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget);
	void WeaponTraceHit(FHitResult& OutHit, const FVector& TraceStart, const FVector& HitTarget);
private:
	
};
