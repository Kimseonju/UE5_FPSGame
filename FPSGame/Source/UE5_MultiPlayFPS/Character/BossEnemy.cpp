// Fill out your copyright notice in the Description page of Project Settings.


#include "BossEnemy.h"
#include "TimerManager.h"
#include "UE5_MultiPlayFPS/MapObject/SkillDecal.h"
#include "UE5_MultiPlayFPS/AI/EnemyAIController.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "UE5_MultiPlayFPS/Character/MultiPlayCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"
#include "UE5_MultiPlayFPS/MultiPlayComponents/ObjectPoolComponent.h"
#include "UE5_MultiPlayFPS/Weapon/Projectile.h"
#include "UE5_MultiPlayFPS/PlayerController/MultiPlayPlayerController.h"
ABossEnemy::ABossEnemy()
{
	ExplodeDamageInnerRadius = 100.f;
	ExplodeDamageOuterRadius = 200.f;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	SceneComp->SetupAttachment(RootComponent);

	Target1 = CreateDefaultSubobject<USceneComponent>("Target1");
	Target1->SetupAttachment(RootComponent);
	Target2 = CreateDefaultSubobject<USceneComponent>("Target2");
	Target2->SetupAttachment(RootComponent);
	FollowTarget = CreateDefaultSubobject<USceneComponent>("FollowTarget");
	FollowTarget->SetupAttachment(RootComponent);

	ObjectPoolCylinder = CreateDefaultSubobject<UObjectPoolComponent>(TEXT("ObjectPoolCylinder"));
	ObjectPoolField = CreateDefaultSubobject<UObjectPoolComponent>(TEXT("ObjectPoolField"));
}

void ABossEnemy::BeginPlay()
{
	Super::BeginPlay();
	LoopTarget1();
}

void ABossEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABossEnemy::ReceiveDamage(AActor* DamageActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser)
{
	Super::ReceiveDamage(DamageActor, Damage, DamageType, InstigatorController, DamageCauser);
	AMultiPlayPlayerController* AttackerController = Cast<AMultiPlayPlayerController>(InstigatorController);
	if (AttackerController)
	{
		AttackerController->SetHUDBossHealthBar(Health, MaxHealth);
	}

}

void ABossEnemy::StartFieldSkill()
{
	GetWorldTimerManager().ClearTimer(FieldSkillHandle);
	GetWorldTimerManager().SetTimer(
		FieldSkillHandle,
		this,
		&ABossEnemy::FieldSkill,
		FieldSkillRecallTimeMax,
		true
	);
}
void ABossEnemy::EndFieldSkill()
{
	GetWorldTimerManager().ClearTimer(FieldSkillHandle);
}

void ABossEnemy::FieldSkill()
{
	float X = FMath::FRandRange(-FieldSkillRange.X, FieldSkillRange.X);
	float Y = FMath::FRandRange(-FieldSkillRange.Y, FieldSkillRange.Y);
	if (SkillDecalClass)
	{
		if (EnemyAIController)
		{
			UObject* TargetObj = EnemyAIController->GetBlackboardComponent()->GetValueAsObject("Target");
			AMultiPlayCharacter* Player = Cast<AMultiPlayCharacter>(TargetObj);
			if (Player)
			{
				FVector Location = Player->GetActorLocation();
				Location.X += X;
				Location.Y += Y;
				ASkillDecal* SkillDecal = Cast<ASkillDecal>(ObjectPoolField->SpawnPoolActor());
				SkillDecal->SetOwner(this);
				SkillDecal->SetInstigator(this);
				SkillDecal->SetActorLocation(Location);
				SkillDecal->PostInit(FieldImpactParticle, FieldImpactSound, BaseDamage * 10.f, 100.f);
			}
		}
	}
}
void ABossEnemy::BossExplode()
{
	UGameplayStatics::ApplyRadialDamageWithFalloff(
		this, BaseDamage*2.f, 10.f, GetActorLocation(),
		ExplodeDamageInnerRadius,
		ExplodeDamageOuterRadius, 1.f, UDamageType::StaticClass()
		, TArray<AActor*>(), this, GetController());
}

void ABossEnemy::BossAttack(BossPattern Pattern)
{
	switch (Pattern)
	{
	case BossPattern::EP_FieldSkill:
		PlayAttackMontage("FieldSkill", 1.f);
		break;

	case BossPattern::EP_EnemyRecall:
		PlayAttackMontage("EnemyRecall", 1.f);
		break;

	case BossPattern::EP_PullTarget:
		PlayAttackMontage("PullTarget", 1.f);
		break;
	case BossPattern::EP_Explode:
		PlayAttackMontage("Explode", 1.f);
		break;
	}
}

void ABossEnemy::BossEnemyRecall()
{
	if (ArrEnemyClass.Num() == 0)
		return;

	int Index = FMath::RandRange(0, ArrEnemyClass.Num() - 1);

	GetWorld()->SpawnActor<AEnemy>(ArrEnemyClass[Index], GetActorLocation() +RecallPoint1, GetActorForwardVector().Rotation());
	GetWorld()->SpawnActor<AEnemy>(ArrEnemyClass[Index], GetActorLocation() + RecallPoint2, GetActorForwardVector().Rotation());
	if (EnemyRecallParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EnemyRecallParticle, GetActorLocation() + RecallPoint1, GetActorForwardVector().Rotation(), true);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EnemyRecallParticle, GetActorLocation() + RecallPoint2, GetActorForwardVector().Rotation(), true);

	}
}

void ABossEnemy::BossFieldSkill()
{
	StartFieldSkill();
}

void ABossEnemy::Dash()
{
	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
	FRotator Rot=GetActorForwardVector().Rotation();
	if (DashEffect)
	{
		DashComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			DashEffect,
			GetRootComponent(),
			FName("None"),
			GetActorLocation(),
			FRotator(0.f, 0.f, Rot.Yaw),
			EAttachLocation::KeepWorldPosition,
			true,
			true
		);
	}
}

void ABossEnemy::DashFinish()
{
	GetCharacterMovement()->MaxWalkSpeed = DashSpeed;
	DashComponent->Deactivate();

	
	
	if (EnemyAIController)
	{
		UObject* TargetObj = EnemyAIController->GetBlackboardComponent()->GetValueAsObject("Target");
		AMultiPlayCharacter* Player=Cast<AMultiPlayCharacter>(TargetObj);
		if (Player)
		{
			Player->Stun();
		}
	}

	DashComponent->DestroyComponent();
	PlayAttackMontage("AttackCombo1", 1.f);
}

void ABossEnemy::DashMove_Fly()
{
	if (!bSaveLocation)
	{
		SaveLocation = GetActorLocation();
		bSaveLocation = true;
	}

	FRotator Rot = GetActorForwardVector().Rotation();
	if (DashEffect)
	{
		DashComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			DashEffect,
			GetRootComponent(),
			FName("None"),
			GetActorLocation(),
			FRotator(0.f, 0.f, Rot.Yaw),
			EAttachLocation::KeepWorldPosition,
			true,
			true
		);
	}

	GetCharacterMovement()->GravityScale = 0.f;

	FLatentActionInfo Info;
	Info.CallbackTarget = this;
	Info.UUID = 0;
	Info.Linkage = 0;
	Info.ExecutionFunction = FName("DashMove_FlyNext");
	int Index = 0;
	for (;;)
	{
		Index = FMath::RandRange(0, BossMove.Num() - 1);
		if (GetActorLocation() != BossMove[Index]->GetActorLocation())
			break;
	}
	
	
	UKismetSystemLibrary::MoveComponentTo(
		GetCapsuleComponent(),
		BossMove[Index]->GetActorLocation(),
		FRotator().ZeroRotator,
		false,
		false,
		1.f,
		false,
		EMoveComponentAction::Type::Move,
		Info);

}

void ABossEnemy::DashMove_FlyNext()
{
	for (int i = 0; i < 10; ++i)
	{
		AProjectile* Projectile=Cast<AProjectile>(ObjectPoolBullet->SpawnPoolActor());
		if (Projectile)
		{
			Projectile->SetOwner(this);
			Projectile->SetInstigator(this);
			FVector Location=SceneComp->GetComponentLocation();
			FVector Target=UKismetMathLibrary::RandomPointInBoundingBox(Location, FVector(200.f,200.f,200.f));
			FVector Forward= Target- Location;
			Forward.Normalize();

			Projectile->SetActorLocation(Location + Forward * 250.f);
			Projectile->SetActorRotation(Forward.Rotation());
			Projectile->SetVelocity(Forward, 1000.f);
		}
	}
}

void ABossEnemy::DashMove_FlyEnd()
{
	bSaveLocation = false;
	SetActorLocation(SaveLocation);
	GetCharacterMovement()->GravityScale = 1.f;
}

void ABossEnemy::FireEvent()
{
	if (EnemyAIController)
	{
		UObject* TargetObj = EnemyAIController->GetBlackboardComponent()->GetValueAsObject("Target");
		AMultiPlayCharacter* Player = Cast<AMultiPlayCharacter>(TargetObj);
		if (Player)
		{
			for (int i = 0; i < 20; ++i)
			{
				AProjectile* Projectile = Cast<AProjectile>(ObjectPoolBullet->SpawnPoolActor());
				if (Projectile)
				{
					Projectile->SetOwner(this);
					Projectile->SetInstigator(this);
					FVector Location = GetActorLocation();
					FVector Target = Player->GetActorLocation();
					FVector Forward = Target - Location;

					Forward.Normalize();

					Projectile->SetActorLocation(Location + Forward * 250.f 
						- FVector(FMath::RandRange(-150.f, 150.f), FMath::RandRange(-150.f, 150.f), FMath::RandRange(-150.f, 150.f)));
					Projectile->SetActorRotation(Forward.Rotation());
					Projectile->SetVelocity(Forward, 2000.f);

				}
			}
		}
	}
}

void ABossEnemy::Firing()
{
	if (EnemyAIController)
	{
		UObject* TargetObj = EnemyAIController->GetBlackboardComponent()->GetValueAsObject("Target");
		AMultiPlayCharacter* Player = Cast<AMultiPlayCharacter>(TargetObj);
		if (Player)
		{
			AProjectile* Projectile = Cast<AProjectile>(ObjectPoolBullet->SpawnPoolActor());
			if (Projectile)
			{
				Projectile->SetOwner(this);
				Projectile->SetInstigator(this);
				FVector Location = GetActorLocation();
				FVector Target = Player->GetActorLocation();
				FVector Forward = Target- Location;
				Forward.Normalize();

				Projectile->SetActorLocation(Location + Forward * 250.f- 
						FVector(FMath::RandRange(-150.f, 150.f), 
								FMath::RandRange(-150.f, 150.f), 
								FMath::RandRange(-150.f, 150.f))
				);
				Projectile->SetActorRotation(Forward.Rotation());
				Projectile->SetVelocity(Forward, 1000.f);
			}
		}
	}
}

void ABossEnemy::LoopTarget1()
{
	FLatentActionInfo Info;
	Info.CallbackTarget = this;
	Info.ExecutionFunction = FName("LoopTarget2");
	Info.UUID = 1;
	Info.Linkage = 0;

	UKismetSystemLibrary::MoveComponentTo(
		FollowTarget,
		Target1->GetRelativeLocation(),
		FRotator().ZeroRotator,
		false,
		false,
		5.f,
		false,
		EMoveComponentAction::Type::Move,
		Info);
}

void ABossEnemy::LoopTarget2()
{
	FLatentActionInfo Info;
	Info.CallbackTarget = this;
	Info.ExecutionFunction = FName("LoopTarget1");
	Info.UUID = 1;
	Info.Linkage = 0;

	UKismetSystemLibrary::MoveComponentTo(
		FollowTarget,
		Target2->GetRelativeLocation(),
		FRotator().ZeroRotator,
		false,
		false,
		5.f,
		false,
		EMoveComponentAction::Type::Move,
		Info);
}

void ABossEnemy::LeftRightFire()
{
	if (EnemyAIController)
	{
		AProjectile* Projectile = Cast<AProjectile>(ObjectPoolBullet->SpawnPoolActor());
		if (Projectile)
		{
			Projectile->SetOwner(this);
			Projectile->SetInstigator(this);
			FVector Location =  SceneComp->GetComponentLocation();
			FVector Target = FollowTarget->GetComponentLocation();
			FVector Forward = Target - Location;
			Forward.Normalize();

			Projectile->SetActorLocation(Location + Forward * 200.f);
			Projectile->SetActorRotation(Forward.Rotation());
			Projectile->SetVelocity(Forward, 1000.f);

		}

	}
}

void ABossEnemy::CircleFire()
{
	for (int i = 0; i < 36; ++i)
	{
		AProjectile* Projectile = Cast<AProjectile>(ObjectPoolBullet->SpawnPoolActor());
		if (Projectile)
		{
			Projectile->SetOwner(this);
			Projectile->SetInstigator(this);
			FVector Location = SceneComp->GetComponentLocation();
			FRotator Rot = FRotator(0.f, i * 10.f, 0.f);
			FVector Forward = Rot.Vector();

			Projectile->SetActorLocation(Location + Forward * 200.f);
			Projectile->SetActorRotation(Rot);
			Projectile->SetVelocity(Forward, 1000.f);

		}
	}
}

void ABossEnemy::CylinderFire()
{
	AProjectile* Projectile = Cast<AProjectile>(ObjectPoolCylinder->SpawnPoolActor());
	if (Projectile)
	{
		Projectile->SetOwner(this);
		Projectile->SetInstigator(this);
		FVector Location = SceneComp->GetComponentLocation();

		Projectile->SetActorLocation(Location);
	}
}

void ABossEnemy::FinishDeath()
{
	Super::FinishDeath();


}

void ABossEnemy::Attack1()
{
	if (EnemyAIController)
	{
		UObject* TargetObj = EnemyAIController->GetBlackboardComponent()->GetValueAsObject("Target");
		AMultiPlayCharacter* Player = Cast<AMultiPlayCharacter>(TargetObj);

		FRotator Forward=UKismetMathLibrary::FindLookAtRotation(SceneComp->GetComponentLocation(), Player->GetActorLocation());
		SetActorRotation(FRotator(0.f, 0.f, Forward.Yaw));
		PlayAttackMontage("AttackCombo1");
	}
}
