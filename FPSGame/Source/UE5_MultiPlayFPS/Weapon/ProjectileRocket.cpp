// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileRocket.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "RocketMovementComponent.h"
#include "Field/FieldSystemComponent.h"
AProjectileRocket::AProjectileRocket()
{
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RocketMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RocketMovementComponent = CreateDefaultSubobject<URocketMovementComponent>(TEXT("RocketMovementComponent"));
	RocketMovementComponent->bRotationFollowsVelocity = true;
	RocketMovementComponent->SetIsReplicated(true);
}


void AProjectileRocket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectileRocket::ObjectPoolInUse(bool Enable)
{
	Super::ObjectPoolInUse(Enable);
	if (Enable)
	{
		if (RocketMovementComponent)
		{
			RocketMovementComponent->Velocity = FVector::ZeroVector;
			RocketMovementComponent->SetUpdatedComponent(GetRootComponent());
			RocketMovementComponent->SetComponentTickEnabled(true);
			ProjectileMesh->SetVisibility(true);
			TrailSystemComponent->GetSystemInstance()->Activate();
			ProjectileLoopComponent->Play();
			//RocketMovementComponent->InitializeComponent();
		}
	}
}

void AProjectileRocket::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &AProjectileRocket::OnHit);
	}
	
	SpawnTrailSystem();
	if (ProjectileLoop && LoopingSoundAttenuation)
	{
		ProjectileLoopComponent = UGameplayStatics::SpawnSoundAttached(
			ProjectileLoop,
			GetRootComponent(),
			FName(),
			GetActorLocation(),
			EAttachLocation::KeepWorldPosition,
			false,
			1.f,
			1.f, 0.f, 
			LoopingSoundAttenuation,
			(USoundConcurrency*)nullptr,
			false);
		
	}
}

void AProjectileRocket::SetVelocity(const FVector& Velocity, float Speed)
{
	RocketMovementComponent->MaxSpeed = Speed;
	RocketMovementComponent->Velocity = Velocity * Speed;
}

void AProjectileRocket::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == GetOwner())
	{
		return;
	}
	RocketMovementComponent->StopMovementImmediately(); 
	ExplodeDamage();
	StartDestroyTimer();
#pragma region Effect
	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform());
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
#pragma endregion
	if (ProjectileMesh)
	{
		ProjectileMesh->SetVisibility(false);
	}
	if (TrailSystemComponent && TrailSystemComponent->GetSystemInstance())
	{
		TrailSystemComponent->GetSystemInstance()->Deactivate();
	}
	if (ProjectileLoopComponent && ProjectileLoopComponent->IsPlaying())
	{
		ProjectileLoopComponent->Stop();
	}
}
