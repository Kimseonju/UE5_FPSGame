#include "ProjectileCircle.h"


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
AProjectileCircle::AProjectileCircle()
{
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RocketMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	DestroyTime = 15.f;
	ScaleSpeed = 1.f;
}


void AProjectileCircle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector Scale=ProjectileMesh->GetComponentScale();
	Scale.Y += ScaleSpeed * DeltaTime;
	Scale.Z += ScaleSpeed * DeltaTime;
	ProjectileMesh->SetWorldScale3D(Scale);
}

void AProjectileCircle::ObjectPoolInUse(bool Enable)
{
	Super::ObjectPoolInUse(Enable);
	if (Enable)
	{
		ProjectileMesh->SetWorldScale3D(SaveScale);
	}
}

void AProjectileCircle::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		ProjectileMesh->OnComponentBeginOverlap.AddDynamic(this, &AProjectileCircle::BeginOverlap);
	}
	SaveScale = ProjectileMesh->GetComponentScale();
	StartDestroyTimer();
}

void AProjectileCircle::SetVelocity(const FVector& Velocity, float Speed)
{
}


void AProjectileCircle::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetOwner())
	{
		return;
	}

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	ACharacter* OtherCharacter = Cast<ACharacter>(OtherActor);
	if (OwnerCharacter && OtherCharacter)
	{
		AController* OwnerController = OwnerCharacter->Controller;

		IGenericTeamAgentInterface* OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(OwnerCharacter->Controller);
		IGenericTeamAgentInterface* OtherTeamInterface = Cast<IGenericTeamAgentInterface>(OtherCharacter->Controller);
		if (OwnerTeamInterface == nullptr || OtherTeamInterface == nullptr || OtherTeamInterface->GetGenericTeamId() == OwnerTeamInterface->GetGenericTeamId()) return;

		if (OwnerController)
		{
			UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, Owner, UDamageType::StaticClass());
		}
	}
	if (OtherCharacter && OwnerCharacter == nullptr)
	{
		UGameplayStatics::ApplyDamage(OtherActor, Damage, nullptr, Owner, UDamageType::StaticClass());
	}
}
