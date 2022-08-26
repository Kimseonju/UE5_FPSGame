// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFrameWork/ProjectileMovementComponent.h"
#include "UE5_MultiPlayFPS/Interface/BulletHitInterface.h"
AProjectileBullet::AProjectileBullet()
{
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
}

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//ProjectileWeapon->CombatComponent 전부 소유자는 플레이어
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	ACharacter* OtherCharacter = Cast<ACharacter>(OtherActor);
	if (OwnerCharacter && OtherCharacter)
	{
		AController* OwnerController = OwnerCharacter->Controller;
		
		IGenericTeamAgentInterface* OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(OwnerCharacter->Controller);
		IGenericTeamAgentInterface* OtherTeamInterface = Cast<IGenericTeamAgentInterface>(OtherCharacter->Controller);
		if (OwnerTeamInterface== nullptr || OtherTeamInterface==nullptr || OtherTeamInterface->GetGenericTeamId() == OwnerTeamInterface->GetGenericTeamId()) return;

		if (OwnerController)
		{
			UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, Owner, UDamageType::StaticClass());
		}
	}
	if (OtherCharacter && OwnerCharacter==nullptr)
	{
		UGameplayStatics::ApplyDamage(OtherActor, Damage, nullptr, Owner, UDamageType::StaticClass());
	}
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}
