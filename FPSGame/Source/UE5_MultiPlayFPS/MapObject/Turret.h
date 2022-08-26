// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Turret.generated.h"

UCLASS()
class UE5_MULTIPLAYFPS_API ATurret : public AActor
{
	GENERATED_BODY()
public:
	ATurret();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* RootCom;
	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* TurretMesh;
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* BeamCheck;
	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* LaserBeam;
	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent* MuzzleFire;
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* Target1;
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* Target2;
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* FollowTarget;

	UPROPERTY(VisibleAnywhere)
	class UObjectPoolComponent* ObjectPoolBullet;

	class UTurretAnimInstace* TurretAnimInstance;

	class AMultiPlayCharacter* Target;

	class USoundCue* ImpactSound;
	FVector BeamTarget;
	FTimerHandle FireHandle;
	UPROPERTY(EditAnywhere)
	float FireTime;
	float TargetTimer;
private:
	UFUNCTION()
	void LoopTarget1();
	UFUNCTION()
	void LoopTarget2();
	void UpdateLaserBeam();
	UFUNCTION()
	void Fire();
	UFUNCTION()
		void BeamBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
