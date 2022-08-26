// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "UE5_MultiPlayFPS/Character/MultiPlayCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "WeaponTypes.h"
#include "UE5_MultiPlayFPS/Interface/BulletHitInterface.h"
#include "UE5_MultiPlayFPS/Character/Enemy.h"
AHitScanWeapon::AHitScanWeapon()
{
	Damage = 20.f;
	DistanceToSphere = 800.f;
	SphereRadius = 75.f;
	bUseScatter = false;
}

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		FHitResult FireHit;
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();
		WeaponTraceHit(FireHit, Start, HitTarget);
		if (FireHit.bBlockingHit)
		{
			//Player
			AMultiPlayCharacter* PlayerCharacter = Cast<AMultiPlayCharacter>(FireHit.GetActor());
			if (PlayerCharacter && HasAuthority() && InstigatorController)
			{
				UGameplayStatics::ApplyDamage(
					PlayerCharacter,
					Damage,
					InstigatorController,
					this,
					UDamageType::StaticClass()
				);
			}
			//BulletHitActor
			IBulletHitInterface* BulletHitInterface = Cast<IBulletHitInterface>(FireHit.GetActor());
			if (BulletHitInterface && HasAuthority() && InstigatorController)
			{
				BulletHitInterface->Execute_BulletHit(FireHit.GetActor(), FireHit, OwnerPawn);
				AEnemy* HitEnemy = Cast<AEnemy>(FireHit.GetActor());
				if (HitEnemy)
				{
					int32 HitDamage = Damage;
					bool bHeadShot = false;
					if (FireHit.BoneName.ToString() == HitEnemy->GetHeadBone())
					{
						Damage = Damage * 2.f;
						bHeadShot = true;
					}
					UGameplayStatics::ApplyDamage(
						FireHit.GetActor(),
						Damage,
						InstigatorController,
						this,
						UDamageType::StaticClass()
					);
				}
			}
		}
#pragma region Particle Sound Spawn
		if (ImpactParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				ImpactParticle,
				FireHit.ImpactPoint,
				FireHit.ImpactNormal.Rotation());
		}
		if (HitSound)
		{
			UGameplayStatics::SpawnSoundAtLocation(
				this,
				HitSound,
				FireHit.ImpactPoint);
		}

		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				MuzzleFlash,
				SocketTransform);
		}
		if (FireSound)
		{
			UGameplayStatics::SpawnSoundAtLocation(
				this,
				FireSound,
				GetActorLocation()
			);
		}
#pragma endregion
	}
}

FVector AHitScanWeapon::TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget)
{
	FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
	FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
	FVector End = SphereCenter + RandVec;
	FVector ToEnd = End - TraceStart;

	return FVector(TraceStart+ ToEnd *TRACE_LENGTH/ToEnd.Size());
}

void AHitScanWeapon::WeaponTraceHit(FHitResult& OutHit, const FVector& TraceStart, const FVector& HitTarget)
{
	UWorld* World = GetWorld();
	if (World)
	{
		//Scatter가 true일시 샷건처럼 공격이 퍼집니다.
		FVector End = bUseScatter ? TraceEndWithScatter(TraceStart, HitTarget) :TraceStart + (HitTarget - TraceStart) * 1.25f;
		World->LineTraceSingleByChannel(
			OutHit,
			TraceStart,
			End,
			ECollisionChannel::ECC_Visibility);
		FVector BeamEnd = End;

		if (OutHit.bBlockingHit)
		{
			BeamEnd = OutHit.ImpactPoint;

		}

		if (BeamParticle)
		{
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
				World,
				BeamParticle,
				TraceStart,
				FRotator::ZeroRotator,
				true);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}
	}
}
