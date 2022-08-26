// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileCircle.generated.h"

/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API AProjectileCircle : public AProjectile
{
	GENERATED_BODY()

public:
	AProjectileCircle();
	virtual void Tick(float DeltaTime) override;
	virtual void ObjectPoolInUse(bool Enable);
protected:
	UFUNCTION()
		void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere)
	float ScaleSpeed;
	FVector SaveScale;
private:
public:
	virtual void SetVelocity(const FVector& Velocity, float Speed);

};
