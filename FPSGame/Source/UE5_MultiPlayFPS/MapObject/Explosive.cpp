#include "Explosive.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
AExplosive::AExplosive()
{
	PrimaryActorTick.bCanEverTick = true;
	ExplosiveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExplosiveMesh"));
	SetRootComponent(ExplosiveMesh);

	OverlapSphere= CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(GetRootComponent());
	BaseDamage = 100.f;
}

void AExplosive::BeginPlay()
{
	Super::BeginPlay();

}

void AExplosive::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExplosive::BulletHit_Implementation(FHitResult HitResult, AActor* HitActor)
{
	ACharacter* Character = Cast<ACharacter>(HitActor);
	if (Character == nullptr)
		return;
#pragma region Effect

	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, HitResult.Location, FRotator(0.f), true);
	}
#pragma endregion
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());
	
	for (auto actor : OverlappingActors)
	{
		UGameplayStatics::ApplyDamage(
			actor,
			BaseDamage,
			Character->GetController(),
			Character,
			UDamageType::StaticClass()

		);
	}
	Destroy();
}

