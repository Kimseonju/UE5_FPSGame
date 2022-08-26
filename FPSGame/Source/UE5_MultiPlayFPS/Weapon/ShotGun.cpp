// Fill out your copyright notice in the Description page of Project Settings.


#include "ShotGun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "UE5_MultiPlayFPS/Character/MultiPlayCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "UE5_MultiPlayFPS/Interface/BulletHitInterface.h"
#include "UE5_MultiPlayFPS/Character/Enemy.h"

AShotGun::AShotGun()
{
	FirePellets = 10;
	bUseScatter = true;
}

void AShotGun::Fire(const FVector& HitTarget)
{
	AWeapon::Fire(HitTarget);
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;

	AController* InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();

		TMap<ACharacter*, int> HitMap;
		for (int i = 0; i < FirePellets; ++i)
		{
			FHitResult FireHit;

			WeaponTraceHit(FireHit, Start, HitTarget);
			if (FireHit.bBlockingHit)
			{
				AMultiPlayCharacter* PlayerCharacter = Cast<AMultiPlayCharacter>(FireHit.GetActor());
				if (PlayerCharacter && HasAuthority() && InstigatorController)
				{
					if (HitMap.Contains(PlayerCharacter))
					{
						HitMap[PlayerCharacter]++;
					}
					else
					{
						HitMap.Emplace(PlayerCharacter, 1);
					}

				}

				IBulletHitInterface* BulletHitInterface = Cast<IBulletHitInterface>(FireHit.GetActor());
				if (BulletHitInterface && HasAuthority() && InstigatorController)
				{
					AEnemy* HitEnemy = Cast<AEnemy>(FireHit.GetActor());
					if (HitEnemy)
					{
						if (HitMap.Contains(HitEnemy))
						{
							HitMap[HitEnemy]++;
						}
						else
						{
							BulletHitInterface->Execute_BulletHit(FireHit.GetActor(), FireHit, Owner);
							HitMap.Emplace(HitEnemy, 1);
						}

					}
				}

			}
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
		}

		for (auto Mapkey : HitMap)
		{
			if (Mapkey.Key && HasAuthority() && InstigatorController)
			{
				UGameplayStatics::ApplyDamage(
					Mapkey.Key,
					Mapkey.Value * Damage,
					InstigatorController,
					this,
					UDamageType::StaticClass()
				);
			}
		}
	}
}
