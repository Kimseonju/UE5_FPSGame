
#include "ProjectileBossBullet.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFrameWork/ProjectileMovementComponent.h"
#include "UE5_MultiPlayFPS/Interface/BulletHitInterface.h"
AProjectileBossBullet::AProjectileBossBullet()
{
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
}

void AProjectileBossBullet::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectileBossBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ObjectPoolInUse(false);
	//ProjectileWeapon->CombatComponent 전부 소유자는 플레이어
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
	MultiCastDestroyEffect(ImpactParticles, ImpactSound);

	//몬스터체크
	IBulletHitInterface* BulletHitInterface = Cast<IBulletHitInterface>(OtherActor);
	if (BulletHitInterface)
	{
		BulletHitInterface->Execute_BulletHit(OtherActor, Hit, Owner);
	}
}
