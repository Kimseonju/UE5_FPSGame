// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"
#include "UE5_MultiPlayFPS/MultiPlayComponents/ObjectPoolComponent.h"

AProjectileWeapon::AProjectileWeapon()
{
	ObjectPoolBullet = CreateDefaultSubobject<UObjectPoolComponent>(TEXT("ObjectPoolComponent"));
	ProjectileSpeed = 2000.f;
}

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);
	if (!HasAuthority()) return;
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();
		if (ProjectileClass && InstigatorPawn)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = InstigatorPawn;
			UWorld* World = GetWorld();
			if (World)
			{
				AProjectile* Projectile=Cast<AProjectile>(ObjectPoolBullet->SpawnPoolActor());
				if (!Projectile)
				{
					UE_LOG(LogTemp, Warning, TEXT("ObjectPoolActor Option Check!"));
					return;
				}
				Projectile->SetOwner(GetOwner());
				Projectile->SetInstigator(InstigatorPawn);
				Projectile->SetActorLocation(SocketTransform.GetLocation());
				Projectile->SetActorRotation(TargetRotation);
				Projectile->SetVelocity(Vec, ProjectileSpeed);
			}
		}
	}
}
