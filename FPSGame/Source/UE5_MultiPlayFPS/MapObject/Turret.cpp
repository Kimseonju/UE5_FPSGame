#include "Turret.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "UE5_MultiPlayFPS/MapObject/AnimInstace/TurretAnimInstace.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "UE5_MultiPlayFPS/Character/MultiPlayCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UE5_MultiPlayFPS/Weapon/Projectile.h"
#include "UE5_MultiPlayFPS/MultiPlayComponents/ObjectPoolComponent.h"
ATurret::ATurret()
{
	PrimaryActorTick.bCanEverTick = true;

	RootCom = CreateDefaultSubobject<USceneComponent>("RootCom");
	SetRootComponent(RootCom);


	TurretMesh = CreateDefaultSubobject<USkeletalMeshComponent>("TurretMesh");
	TurretMesh->SetupAttachment(RootCom);
	BeamCheck = CreateDefaultSubobject<UBoxComponent>("BeamCheck");
	BeamCheck->SetupAttachment(TurretMesh, FName(TEXT("Circle_002")));
	LaserBeam = CreateDefaultSubobject<UNiagaraComponent>("LaserBeam");
	LaserBeam->SetupAttachment(TurretMesh, FName(TEXT("Circle_002")));
	MuzzleFire = CreateDefaultSubobject<UParticleSystemComponent>("MuzzleFire");
	MuzzleFire->SetupAttachment(TurretMesh);

	Target1 = CreateDefaultSubobject<USceneComponent>("Target1");
	Target1->SetupAttachment(RootCom);
	Target2 = CreateDefaultSubobject<USceneComponent>("Target2");
	Target2->SetupAttachment(RootCom);
	FollowTarget = CreateDefaultSubobject<USceneComponent>("FollowTarget");
	FollowTarget->SetupAttachment(RootCom);

	ObjectPoolBullet = CreateDefaultSubobject<UObjectPoolComponent>("ObjectPoolBullet");
	FireTime = 0.3f;
}
void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Target)
	{
		BeamTarget = FMath::Lerp(Target->GetActorLocation(), BeamTarget, 0.f);
	}
	else
	{
		BeamTarget = FMath::Lerp(FollowTarget->GetComponentLocation(), BeamTarget, 0.f);
	}
	if (TurretAnimInstance)
	{
		FVector SocketLocation = TurretMesh->GetSocketLocation(FName("BodySocket"));
		
		TurretAnimInstance->SetLookAtRotation(UKismetMathLibrary::FindLookAtRotation(SocketLocation, BeamTarget));
	}

	UpdateLaserBeam();
}

void ATurret::BeginPlay()
{
	Super::BeginPlay();

	TurretAnimInstance =Cast<UTurretAnimInstace>(TurretMesh->GetAnimInstance());
	if (TurretAnimInstance)
	{
		LoopTarget1();
	}
	BeamCheck->OnComponentBeginOverlap.AddDynamic(this, &ATurret::BeamBeginOverlap);
	FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);

	BeamCheck->AttachToComponent(TurretMesh, Rule,FName("Circle_002"));
	LaserBeam->AttachToComponent(TurretMesh, Rule, FName("Circle_002"));

}

void ATurret::LoopTarget1()
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

void ATurret::LoopTarget2()
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

void ATurret::UpdateLaserBeam()
{
	FVector Start= TurretMesh->GetSocketLocation(FName("Circle_002"));
	FVector End = FollowTarget->GetComponentLocation();

	if (Target)
	{
		LaserBeam->SetVectorParameter(FName("User.Beam End"), Target->GetActorLocation());
	}
	else
	{
		LaserBeam->SetVectorParameter(FName("User.Beam End"), End);
	}
}

void ATurret::Fire()
{
	if (!Target) return;

	FVector Start = TurretMesh->GetSocketLocation(FName("Circle_002"));
	FVector End = Target->GetActorLocation();

	if ((Start - End).Length() > (Start - FollowTarget->GetComponentLocation()).Length())
	{
		Target = nullptr;
		GetWorldTimerManager().ClearTimer(FireHandle);
		return;
	}
	MuzzleFire->Activate(true);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, Start);

	FHitResult HitResult;
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(HitResult,
		Start,
		End,
		ECollisionChannel::ECC_Visibility,
		queryParams);

	if (HitResult.bBlockingHit)
	{
		FVector Forward = End - Start;
		Forward.Normalize();

		AProjectile* Actor = Cast<AProjectile>(ObjectPoolBullet->SpawnPoolActor());
		if (!Actor)
		{
			UE_LOG(LogTemp, Warning, TEXT("ObjectPoolActor Option Check!"));
			return;
		}
		Actor->SetOwner(this);
		Actor->SetActorLocation(Start);
		Actor->SetActorRotation(Forward.Rotation());
		Actor->SetVelocity(Forward, 15000.f);


		if (HitResult.GetActor() != Target)
		{
			GetWorldTimerManager().ClearTimer(FireHandle);
			Target = nullptr;
			return;
		}
	}
	else
	{
		Target = nullptr;
		GetWorldTimerManager().ClearTimer(FireHandle);
	}
}

void ATurret::BeamBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMultiPlayCharacter* TargetCharacter = Cast<AMultiPlayCharacter>(OtherActor);
	if (!TargetCharacter) return;
	Target=TargetCharacter;
	FVector Start = TurretMesh->GetSocketLocation(FName("Circle_002"));
	FVector End = OtherActor->GetActorLocation();

	FHitResult HitResult;
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(HitResult,
		Start,
		End,
		ECollisionChannel::ECC_Visibility,
		queryParams);
	if (HitResult.bBlockingHit)
	{
		Fire();
		GetWorldTimerManager().SetTimer(
			FireHandle,
			this,
			&ATurret::Fire,
			FireTime,
			true
		);
	}
}
