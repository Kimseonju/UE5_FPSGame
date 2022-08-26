#include "ProjectileSmokeGrenade.h"
#include "Kismet/GameplayStatics.h"
#include "GameFrameWork/ProjectileMovementComponent.h"
#include "Sound/SoundCue.h"
#include "Field/FieldSystemComponent.h"

AProjectileSmokeGrenade::AProjectileSmokeGrenade()
{
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrenadeMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("GrenadeMovement"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
	ProjectileMovementComponent->bShouldBounce = true;
}

void AProjectileSmokeGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectileSmokeGrenade::ObjectPoolInUse(bool Enable)
{
	if (!Enable)
	{
		BlueprintDestoryUpdate();
	}
}

void AProjectileSmokeGrenade::BeginPlay()
{
	AActor::BeginPlay();
	SpawnTrailSystem();
	StartDestroyTimer();

	ProjectileMovementComponent->OnProjectileBounce.AddDynamic(this, &AProjectileSmokeGrenade::OnBouse);
}

void AProjectileSmokeGrenade::OnBouse(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
}

void AProjectileSmokeGrenade::Destroyed()
{
	Super::Destroyed();
}
