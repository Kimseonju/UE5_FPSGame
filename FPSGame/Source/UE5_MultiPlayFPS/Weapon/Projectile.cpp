#include "Projectile.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "UE5_MultiPlayFPS/Character/MultiPlayCharacter.h"
#include "UE5_MultiPlayFPS/UE5_MultiPlayFPS.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "UE5_MultiPlayFPS/Interface/BulletHitInterface.h"
#include "Field/FieldSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

	FieldSystemComponent = CreateDefaultSubobject<UFieldSystemComponent>(TEXT("FieldSystemComponent"));
	FieldSystemComponent->SetupAttachment(RootComponent);
	FieldSystemComponent->SetSimulatePhysics(true);
	DestroyTime = 3.f;
	Damage = 20.f;
	DamageInnerRadius = 200.f;
	DamageOuterRadius = 500.f;
	bInuse = false;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	if (Tracer)
	{
		TracerComponent = UGameplayStatics::SpawnEmitterAttached(Tracer, CollisionBox, FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition
		);
	}
	if (HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	}	
}

void AProjectile::StartDestroyTimer()
{
	GetWorldTimerManager().SetTimer(
		DestroyTimer,
		this,
		&AProjectile::DestroyTimerFinished,
		DestroyTime);

}

void AProjectile::DestroyTimerFinished()
{
	//MultiCastDestroyEffect(ImpactParticles, ImpactSound);
	//Destroy();
	ObjectPoolInUse(false);
}

void AProjectile::ExplodeDamage()
{
	APawn* FirePawn = GetInstigator();
	if (FirePawn && HasAuthority())
	{
		AController* FireController = FirePawn->GetController();
		if (FireController)
		{
			UGameplayStatics::ApplyRadialDamageWithFalloff(
				this, Damage, 10.f, GetActorLocation(),
				DamageInnerRadius,
				DamageOuterRadius, 1.f, UDamageType::StaticClass()
				, TArray<AActor*>(), this, FireController);

		}
	}

	ACharacter* PlayerCharacter=UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (PlayerCharacter)
	{
		AMultiPlayCharacter* MultiCharacter = Cast<AMultiPlayCharacter>(PlayerCharacter);
		if (MultiCharacter)
		{
			MultiCharacter->StartCameraShakeBoom(1.f);
		}
	}


}

void AProjectile::SpawnTrailSystem()
{
	if (TrailSystem)
	{
		TrailSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			TrailSystem,
			GetRootComponent(),
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition,
			false);
	}
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == GetOwner())
	{
		return;
	}
	MultiCastDestroyEffect(ImpactParticles, ImpactSound);

	//몬스터체크
	IBulletHitInterface* BulletHitInterface = Cast<IBulletHitInterface>(OtherActor);
	if (BulletHitInterface)
	{
		BulletHitInterface->Execute_BulletHit(OtherActor, Hit, Owner);
	}
	ObjectPoolInUse(false);
}

void AProjectile::SetVelocity(const FVector& Velocity, float Speed)
{
	ProjectileMovementComponent->MaxSpeed = Speed;
	ProjectileMovementComponent->Velocity = Velocity* Speed;
}

void AProjectile::MultiCastDestroyEffect_Implementation(UParticleSystem* Particles, USoundCue* Sound)
{
	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Particles, GetActorTransform());
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, Sound, GetActorLocation());
	}
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::Destroyed()
{
	Super::Destroyed();
}

void AProjectile::ObjectPoolInUse(bool Enable)
{
	Super::ObjectPoolInUse(Enable);
	if (Enable)
	{
		if (ProjectileMovementComponent)
		{
			ProjectileMovementComponent->Velocity = FVector::ZeroVector;
			ProjectileMovementComponent->SetUpdatedComponent(GetRootComponent());
			ProjectileMovementComponent->SetComponentTickEnabled(true);
		}
	}
}
