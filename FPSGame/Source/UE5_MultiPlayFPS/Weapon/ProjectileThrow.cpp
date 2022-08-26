// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileThrow.h"

// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileGrenade.h"
#include "Kismet/GameplayStatics.h"
#include "GameFrameWork/ProjectileMovementComponent.h"
#include "Sound/SoundCue.h"
#include "Field/FieldSystemComponent.h"

AProjectileThrow::AProjectileThrow()
{
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrenadeMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("GrenadeMovement"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
	ProjectileMovementComponent->bShouldBounce = true;
}

void AProjectileThrow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectileThrow::ObjectPoolInUse(bool Enable)
{
	if (IsUse() && !Enable)
	{
		ExplodeDamage();
		if (ImpactParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform());
		}
		if (ImpactSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
		}
		Destroy();
		return;
	}
}

void AProjectileThrow::BeginPlay()
{
	AActor::BeginPlay();
	SpawnTrailSystem();
	StartDestroyTimer();
	bInuse = true;
}

void AProjectileThrow::OnBouse(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	ObjectPoolInUse(false);
}

void AProjectileThrow::Destroyed()
{
	Super::Destroyed();
}
