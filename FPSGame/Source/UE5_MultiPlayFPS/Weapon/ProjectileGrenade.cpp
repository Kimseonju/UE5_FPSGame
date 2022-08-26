// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileGrenade.h"
#include "Kismet/GameplayStatics.h"
#include "GameFrameWork/ProjectileMovementComponent.h"
#include "Sound/SoundCue.h"
#include "Field/FieldSystemComponent.h"

AProjectileGrenade::AProjectileGrenade()
{
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrenadeMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("GrenadeMovement"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
	ProjectileMovementComponent->bShouldBounce = true;
}

void AProjectileGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectileGrenade::ObjectPoolInUse(bool Enable)
{
	if (!Enable)
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
	}
	Super::ObjectPoolInUse(Enable);
}

void AProjectileGrenade::BeginPlay()
{
	AActor::BeginPlay();
	StartDestroyTimer();
	ProjectileMovementComponent->OnProjectileBounce.AddDynamic(
		this, 
		&AProjectileGrenade::OnBouse
	);
}

void AProjectileGrenade::OnBouse(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	if (BouseSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			BouseSound,
			GetActorLocation());
	}
}

void AProjectileGrenade::Destroyed()
{
	Super::Destroyed();
}
