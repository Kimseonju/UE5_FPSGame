// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UE5_MultiPlayFPS/Interface/BulletHitInterface.h"
#include "Explosive.generated.h"

UCLASS()
class UE5_MULTIPLAYFPS_API AExplosive : public AActor, public IBulletHitInterface
{
	GENERATED_BODY()
	
public:	
	AExplosive();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = Combat)
	class UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere,  Category = Combat)
	class USoundCue* ImpactSound;

	UPROPERTY(VisibleAnywhere,  Category = Combat)
	class UStaticMeshComponent* ExplosiveMesh;

	UPROPERTY(VisibleAnywhere, Category = Combat)
	class USphereComponent* OverlapSphere;

	UPROPERTY(EditAnywhere, Category = Combat)
	float BaseDamage;
public:	
	virtual void Tick(float DeltaTime) override;

	virtual void BulletHit_Implementation(FHitResult HitResult, AActor* HitActor) override;
	
};
