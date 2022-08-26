#include "SkillDecal.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
ASkillDecal::ASkillDecal()
{
 	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	Circum= CreateDefaultSubobject<UDecalComponent>("Circum");
	Area = CreateDefaultSubobject<UDecalComponent>("Area");
	Circum->SetupAttachment(GetRootComponent());
	Area->SetupAttachment(GetRootComponent());
	LifeTimeEnd = 1.f;
	LifeTime = 0.f;
	Radius = 0.8f;
	bCircularSector = false;
	SetActorRotation(FRotator(-90.f, 0.f, 0.f));
}

void ASkillDecal::BeginPlay()
{
	Super::BeginPlay();
	if (Circum)
	{
		CircumInstance =Circum->CreateDynamicMaterialInstance();
		if (CircumInstance)
		{
			CircumInstance->SetScalarParameterValue("Circum1Area0", 1.f);
		}
	}
	if (Area)
	{
		AreaInstance = Area->CreateDynamicMaterialInstance();
		if (AreaInstance)
		{
			AreaInstance->SetScalarParameterValue("Circum1Area0", 0.f);
		}
	}
	if (bCircularSector)
	{
		if (CircumInstance && AreaInstance)
		{
			CircumInstance->SetScalarParameterValue("RadianValue", Radius);
			AreaInstance->SetScalarParameterValue("RadianValue", Radius);
		}
	}
}

void ASkillDecal::Reset()
{
	LifeTime = 0.f;
}

void ASkillDecal::PostInit(class UParticleSystem* _ImpactParticle, class USoundCue* _ImpactSound
						, float _Damage, float _Radius)
{
	ImpactParticle = _ImpactParticle;
	ImpactSound = _ImpactSound;
	Damage = _Damage;
	Radius = _Radius;
}

void ASkillDecal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	LifeTime += DeltaTime;
	//0~0.5f
	float AreaPercent = LifeTime / LifeTimeEnd / 2.f;
	if (AreaInstance)
		AreaInstance->SetScalarParameterValue("AreaValue", AreaPercent);
	if (AreaPercent >= 0.5f)
	{
		APawn* OwnerPawn = GetInstigator();
		if (OwnerPawn && HasAuthority())
		{
			AController* FireController = OwnerPawn->GetController();
			if (FireController)
			{
				TArray<AActor*> ArrActor;
				ArrActor.Push(OwnerPawn);
				UGameplayStatics::ApplyRadialDamage(
					this,
					Damage,
					GetActorLocation(),
					Radius,
					UDamageType::StaticClass(),
					ArrActor,
					OwnerPawn,
					FireController,
					false,
					ECollisionChannel::ECC_Visibility
				);

			}
		}
#pragma region Effect
		if (ImpactSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
		}
		if (ImpactParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, GetActorLocation());
		}
#pragma endregion
		ObjectPoolInUse(false);
	}
}

void ASkillDecal::ObjectPoolInUse(bool Enable)
{
	Super::ObjectPoolInUse(Enable);
	if (!Enable)
	{
		Reset();
	}
}
