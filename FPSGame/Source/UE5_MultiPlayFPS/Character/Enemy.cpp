#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/WidgetComponent.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"
#include "DrawDebugHelpers.h"
#include "UE5_MultiPlayFPS/AI/EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SphereComponent.h"
#include "MultiPlayCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "UE5_MultiPlayFPS/GameMode/MultiPlayGameMode.h"
#include "UE5_MultiPlayFPS/Widget/HUD/EnemyHitNumberWidget.h"
#include "UE5_MultiPlayFPS/MultiPlayComponents/ObjectPoolComponent.h"
#include "UE5_MultiPlayFPS/PlayerController/MultiPlayPlayerController.h"
#include "UE5_MultiPlayFPS/Weapon/Projectile.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	MaxHealth = 100.f;
	Health = 100.f;

	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarWidget"));
	HPBarWidget->SetupAttachment(RootComponent);
	HPBarWidget->SetVisibility(false);

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(RootComponent);

	CombatRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatRangeSphere"));
	CombatRangeSphere->SetupAttachment(RootComponent);

	LeftWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftWeaponCollision"));

	RightWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightWeaponCollision"));

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	
	LeftWeaponCollision->SetupAttachment(GetMesh(), FName("LeftWeaponBoneSocket"));
	RightWeaponCollision->SetupAttachment(GetMesh(), FName("RightWeaponBoneSocket"));

	LeftWeaponCollision->SetRelativeLocation(FVector(30.f, -17.f, 0.f));
	RightWeaponCollision->SetRelativeLocation(FVector(-4.f, 15.f, 2.f));
	EnableCustomDepth(false);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_ENEMYBLUE);

	AIPerComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerComp"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	SightConfig->SightRadius = 800.f;
	SightConfig->LoseSightRadius = 1000.f;
	SightConfig->PeripheralVisionAngleDegrees = 90.0f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
	SightConfig->SetMaxAge(0.1f);


	AIPerComp->ConfigureSense(*SightConfig);
	AIPerComp->SetDominantSense(SightConfig->GetSenseImplementation());

	ObjectPoolBullet = CreateDefaultSubobject<UObjectPoolComponent>(TEXT("ObjectPoolComponent"));

	HealthBarDisplayTime = 3.f;
	bCanHitReact = true;
	HitReactTime = 1.f;
	HitNumberDestroyTime = 1.5f;
	bStunned = false;
	StunChance = 0.5f;
	bCanAttack = true;
	AttackWaitTime = 1.f;
	bDied = false;
	DeathTime = 4.f;
	bFireCharacter = false;
	StunTime = 1.f;
	ScanDepthTime = 2.f;
	bDieRegdoll = true;

	FireRange = 100.f;
	FireTime = 0.3f;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	
	LeftWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Ignore);
	RightWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Ignore);
	AgroSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Ignore);
	CombatRangeSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Ignore);
	
	OnTakeAnyDamage.AddDynamic(this, &AEnemy::ReceiveDamage);
	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOverlap);
	CombatRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatRangeSphereStartOverlap);
	CombatRangeSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatRangeSphereEndOverlap);

	LeftWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnLeftWeaponOverlap);
	RightWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnRightWeaponOverlap);

	EnemyAIController = Cast<AEnemyAIController>(GetController());
	if (EnemyAIController)
	{
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"), bCanAttack);
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsDeath"), false);
	}

	if (EnemyAIController)
	{
		EnemyAIController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint"), GetActorLocation()+PatrolPoint);
		EnemyAIController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint2"), GetActorLocation()+PatrolPoint2);
		EnemyAIController->RunBehaviorTree(BehaviorTree);
	}
}

void AEnemy::Die()
{
	if (bDied)
		return;
	bDied = true;
	HideHealthBar();
	PlayDieMontage();
	if (EnemyAIController)
	{
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsDeath"), true);
		//EnemyAIController->StopMovement();
	}
}

void AEnemy::PlayHitMontage(FName Section, float PlayRate)
{
	if (bCanHitReact)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && HitMontage)
		{
			AnimInstance->Montage_Play(HitMontage, PlayRate);
			AnimInstance->Montage_JumpToSection(Section, HitMontage);
		}
		bCanHitReact = false;

		GetWorldTimerManager().ClearTimer(HitReactTimer);
		GetWorldTimerManager().SetTimer(
			HitReactTimer,
			this,
			&AEnemy::ResetHitReactTimer,
			HitReactTime
		);
		HPBarWidget->SetVisibility(true);
	}
}

void AEnemy::PlayDieMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
		//AnimInstance->Montage_JumpToSection(Section, HitMontage);
	}
}

void AEnemy::ResetHitReactTimer()
{
	bCanHitReact = true;
}

void AEnemy::CreateHitNumberWidget(const int32 Damage, const FVector HitLocation, const bool bHeadShot)
{
	if (HitNumberWidgetClass == nullptr) return;
	UEnemyHitNumberWidget* HitNumberWidget = CreateWidget<UEnemyHitNumberWidget>(GetWorld(), HitNumberWidgetClass);
	HitNumberWidget->AddToViewport();
	HitNumberWidget->SetHitNumber(Damage, bHeadShot);
	FVector2D ScreenPosition;
	UGameplayStatics::ProjectWorldToScreen(
		GetWorld()->GetFirstPlayerController(),
		HitLocation,
		ScreenPosition
	);
	HitNumberWidget->SetPositionInViewport(ScreenPosition);
	StoreHitNumber(HitNumberWidget, HitLocation);
}

void AEnemy::StoreHitNumber(UUserWidget* HitNumber, FVector Location)
{
	HitNumbers.Add(HitNumber, Location);
	FTimerHandle HitNumberTimer;
	FTimerDelegate HitNumberDelegate;
	HitNumberDelegate.BindUFunction(this, FName("DestroyHitNumber"), HitNumber);
	GetWorldTimerManager().SetTimer(
		HitNumberTimer,
		HitNumberDelegate,
		HitNumberDestroyTime,
		false
	);
}
void AEnemy::DestroyHitNumber(UUserWidget* HitNumber)
{
	HitNumbers.Remove(HitNumber);
	HitNumber->RemoveFromParent();
}

void AEnemy::UpdateHitNumbers()
{
	for (auto& HitPair : HitNumbers)
	{
		UUserWidget* HitNumber= HitPair.Key;
		FVector Location = HitPair.Value;
		FVector2D ScreenPosition;
		UGameplayStatics::ProjectWorldToScreen(
			GetWorld()->GetFirstPlayerController(),
			Location,
			ScreenPosition
		);
		HitNumber->SetPositionInViewport(ScreenPosition);
	}
}

void AEnemy::AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;
	AMultiPlayCharacter* Character = Cast<AMultiPlayCharacter>(OtherActor);
	if (Character)
	{
		if (EnemyAIController->GetBlackboardComponent())
		{
			EnemyAIController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), Character);
		}
	}
}

void AEnemy::CombatRangeSphereStartOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;
	AMultiPlayCharacter* Character = Cast<AMultiPlayCharacter>(OtherActor);

	if (Character)
	{
		bInAttackRange = true;
		if (EnemyAIController)
		{
			EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsAttackRange"), bInAttackRange);
		}
	}
}


void AEnemy::AttackDamage(AActor* Victim)
{
	UGameplayStatics::ApplyDamage(
		Victim,
		BaseDamage,
		EnemyAIController,
		this,
		UDamageType::StaticClass()
	);
}

void AEnemy::CombatRangeSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == nullptr) return;
	AMultiPlayCharacter* Character = Cast<AMultiPlayCharacter>(OtherActor);

	if (Character)
	{
		bInAttackRange = false;
		if (EnemyAIController)
		{
			EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsAttackRange"), bInAttackRange);
		}
	}
}

void AEnemy::OnLeftWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr)return;
	AMultiPlayCharacter* Character = Cast<AMultiPlayCharacter>(OtherActor);
	if (Character)
	{
		AttackDamage(Character);
		StunCharacter(Character);
	}
}

void AEnemy::OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr)return;
	AMultiPlayCharacter* Character = Cast<AMultiPlayCharacter>(OtherActor);
	if (Character)
	{
		AttackDamage(Character);
		StunCharacter(Character);
	}
}

void AEnemy::ActivateLeftWeapon()
{
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateLeftWeapon()
{
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::ActivateRightWeapon()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateRightWeapon()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::FinishDeath()
{
	GetMesh()->bPauseAnims = true;
	if (bDieRegdoll)
	{
		GetMesh()->SetSimulatePhysics(true);
	}
	GetWorldTimerManager().SetTimer(
		DeathTimer,
		this,
		&AEnemy::DestroyEnemy,
		DeathTime
	);
}

void AEnemy::FinishStun()
{
	SetStunned(false);
}

void AEnemy::Fire()
{
	if (EnemyAIController)
	{
		UObject* TargetObj=EnemyAIController->GetBlackboardComponent()->GetValueAsObject("Target");
		AActor* Target = Cast<AActor>(TargetObj);
		if (Target)
		{
			if (ProjectileClass)
			{
				float RangeX = FMath::FRandRange(0.f, FireRange);
				float RangeY = FMath::FRandRange(0.f, FireRange);
				float RangeZ = FMath::FRandRange(0.f, FireRange);
				FVector MuzzleLocation = GetMesh()->GetSocketLocation("Muzzle_01");
				FVector TargetLocation = Target->GetActorLocation();
				TargetLocation.X += RangeX;
				TargetLocation.Y += RangeY;
				TargetLocation.Z += RangeZ;
				FVector Forward = TargetLocation - MuzzleLocation;
				Forward.Normalize();
				//Set Spawn Collision Handling Override
				// spawn the projectile at the muzzle
				AProjectile* Actor=Cast<AProjectile>(ObjectPoolBullet->SpawnPoolActor());
				if (!Actor)
				{
					UE_LOG(LogTemp, Warning, TEXT("ObjectPoolActor Option Check!"));
					return;
				}
				Actor->SetOwner(this);
				Actor->SetInstigator(this);
				Actor->SetActorLocation(MuzzleLocation);
				Actor->SetActorRotation(Forward.Rotation());
				Actor->SetVelocity(Forward, 15000.f);
			}
		}
		
	}
}


void AEnemy::Scanning()
{
	EnableCustomDepth(true);
	GetWorldTimerManager().SetTimer(
		ScanDepthTimer,
		this,
		&AEnemy::FinishScanTimer,
		ScanDepthTime
	);
}

void AEnemy::StunCharacter(class AMultiPlayCharacter* Victim)
{
	if (Victim)
	{
		const float stun = FMath::FRandRange(0.f, 1.f);
		if (stun <= Victim->GetStunChance())
		{
			Victim->Stun();
		}
	}
}

void AEnemy::ResetCanAttack()
{
	bCanAttack = true;
	if (EnemyAIController)
	{
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"), bCanAttack);
	}
}

void AEnemy::DestroyEnemy()
{
	Destroy();
}

void AEnemy::RegdollEvent()
{

}

void AEnemy::EnableCustomDepth(bool bEnable)
{
	if (GetMesh())
	{
		GetMesh()->SetRenderCustomDepth(bEnable);
	}
	
}

void AEnemy::FinishScanTimer()
{
	EnableCustomDepth(false);
}


void AEnemy::Firing()
{
}

void AEnemy::PlayAttackMontage(FName Section, float PlayRate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage, PlayRate);
		AnimInstance->Montage_JumpToSection(Section, AttackMontage);
	}
	if (EnemyAIController)
	{
		UObject* TargetObj = EnemyAIController->GetBlackboardComponent()->GetValueAsObject("Target");
		AActor* Target = Cast<AActor>(TargetObj);
		if (Target)
		{
			FRotator Rot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation());
			Rot.Pitch = 0.f;
			SetActorRotation(Rot);
		}
	}

	bCanAttack = false;
	GetWorldTimerManager().SetTimer(
		AttackWaitTimer,
		this,
		&AEnemy::ResetCanAttack,
		AttackWaitTime
	);
	if (EnemyAIController)
	{
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"), bCanAttack);
	}
}

FName AEnemy::GetAttackSectionName()
{
	FName SectionName;
	const int Section = FMath::RandRange(0, 2);
	switch (Section)
	{
	case 0:
		SectionName = TEXT("AttackCombo1");
		break;

	case 1:
		SectionName = TEXT("AttackCombo2");
		break;

	case 2:
		SectionName = TEXT("AttackCombo3");
		break;
	}
	return SectionName;
}

void AEnemy::SetStunned(bool Stunned)
{
	bStunned = Stunned;
	if (EnemyAIController)
	{
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("Stunned"), Stunned);
	}
	if (bStunned)
	{
		GetWorldTimerManager().ClearTimer(StunTimer);
		GetWorldTimerManager().SetTimer(
			StunTimer,
			this,
			&AEnemy::FinishStun,
			StunTime
		);
	}
}

void AEnemy::ShowHealthBar()
{
	if (HPBarWidget)
	{
		GetWorldTimerManager().ClearTimer(HealthBarTimer);
		GetWorldTimerManager().SetTimer(
			HealthBarTimer,
			this,
			&AEnemy::HideHealthBar,
			HealthBarDisplayTime
		);
		HPBarWidget->SetVisibility(true);
	}
	
}

void AEnemy::HideHealthBar()
{
	if (HPBarWidget)
	{
		GetWorldTimerManager().ClearTimer(HealthBarTimer);
		HPBarWidget->SetVisibility(false);
	}

}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateHitNumbers();

}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::BulletHit_Implementation(FHitResult HitResult, AActor* HitActor)
{
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, HitResult.Location, FRotator(0.f), true);
	}
}

void AEnemy::ReceiveDamage(AActor* DamageActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser)
{

	AMultiPlayPlayerController* AttackerController = Cast<AMultiPlayPlayerController>(InstigatorController);
	if (AttackerController)
	{
		AttackerController->ShowCross();
	}
	else
	{
		return;
	}
	if (bDied) return;
	CreateHitNumberWidget(Damage, GetActorLocation(), false);
	

	if (EnemyAIController && AttackerController)
	{
		UObject* Obj=EnemyAIController->GetBlackboardComponent()->GetValueAsObject(FName("Target"));
		if (Obj != DamageCauser)
			EnemyAIController->GetBlackboardComponent()->SetValueAsObject(FName("Target"), DamageCauser);
	}
	if (Health - Damage <= 0.f)
	{
		Health = 0.f;
		Die();
		AMultiPlayGameMode* PlayGameMode = GetWorld()->GetAuthGameMode<AMultiPlayGameMode>();

		if (PlayGameMode)
		{
			if(AttackerController)
				PlayGameMode->PlayerKillEnemy(1, AttackerController);
		}

		return;
	}
	else
	{
		Health -= Damage;
	}
	ShowHealthBar();
	

}
