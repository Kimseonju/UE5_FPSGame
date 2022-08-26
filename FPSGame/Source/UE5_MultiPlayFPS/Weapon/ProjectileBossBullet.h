// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileBossBullet.generated.h"

/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API AProjectileBossBullet : public AProjectile
{
	GENERATED_BODY()
public:
	AProjectileBossBullet();

protected:
	virtual void BeginPlay() override;
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
