// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "ProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	AProjectileWeapon();
	virtual void Fire(const FVector& HitTarget) override;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Info", meta=(AllowPrivateAccess="true"))
	class UObjectPoolComponent* ObjectPoolBullet;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere)
	float ProjectileSpeed;

};
