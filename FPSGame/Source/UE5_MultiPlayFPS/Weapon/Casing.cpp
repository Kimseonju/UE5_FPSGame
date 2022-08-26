#include "Casing.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
ACasing::ACasing()
{
	PrimaryActorTick.bCanEverTick = false;

	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	SetRootComponent(CasingMesh);
	CasingMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	CasingMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	CasingMesh->SetSimulatePhysics(true);
	CasingMesh->SetEnableGravity(true);
	CasingMesh->SetNotifyRigidBodyCollision(true);
	ShellEjectionImpulse = 7.f;
	DestroyTimer = 1.f;
	bStartTimer = false;
}

void ACasing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CasingMesh->AddLocalRotation(FQuat(RandomRotator));
	if (bStartTimer)
	{
		DestroyTimer -= DeltaTime;
		if (DestroyTimer <= 0.f)
		{
			Destroy();
		}
	}
}

void ACasing::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ShellSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
	}
	RandomRotator = FRotator(0.f,0.f,0.f);
	bStartTimer = true;
	//충돌 알림 끄기
	CasingMesh->SetNotifyRigidBodyCollision(false);
}

void ACasing::BeginPlay()
{
	Super::BeginPlay();
	CasingMesh->OnComponentHit.AddDynamic(this, &ACasing::OnHit);
	CasingMesh->AddImpulse(GetActorForwardVector()* ShellEjectionImpulse);
	RandomRotator = UKismetMathLibrary::RandomRotator(true);
	RandomRotator.Normalize();
}