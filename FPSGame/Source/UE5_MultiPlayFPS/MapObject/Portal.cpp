// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"
#include "Components/SplineComponent.h"
#include "Components/BoxComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineMeshComponent.h"
APortal::APortal()
{
	PrimaryActorTick.bCanEverTick = true;

	RootCom = CreateDefaultSubobject<USceneComponent>("RootCom");
	SetRootComponent(RootCom);

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
	Spline->SetupAttachment(RootCom);

	Door1 = CreateDefaultSubobject<UStaticMeshComponent>("Door1");
	Door1->SetupAttachment(RootCom);
	Door1->SetVisibility(false);
	Door2 = CreateDefaultSubobject<UStaticMeshComponent>("Door2");
	Door2->SetupAttachment(RootCom);
	Door2->SetVisibility(false);
	BoxCol1 = CreateDefaultSubobject<UBoxComponent>("BoxCol1");
	BoxCol1->SetupAttachment(RootCom);

	BoxCol2 = CreateDefaultSubobject<UBoxComponent>("BoxCol2");
	BoxCol2->SetupAttachment(RootCom);
	bReversePortal = false;
	bPortal = false;
}

void APortal::BeginPlay()
{
	Super::BeginPlay();
	BoxCol1->OnComponentBeginOverlap.AddDynamic(this, &APortal::Box1CollisionBeginOverlap);
	BoxCol2->OnComponentBeginOverlap.AddDynamic(this, &APortal::Box2CollisionBeginOverlap);
	PortalOpen();
}

void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void APortal::PortalOpen()
{
	bPortal = true;
	bReversePortal = true;
	Door1->SetVisibility(true);
	Door2->SetVisibility(true);

	GetWorldTimerManager().SetTimer(
		ReversePartolTimer,
		this,
		&APortal::ReversePortalTimerFinish,
		1.f
	);

	GetWorldTimerManager().SetTimer(
		PartolTimer,
		this,
		&APortal::PortalTimerFinish,
		1.f
	);

	Spline->Activate();
	Door1->Activate();
	Door2->Activate();
	BoxCol1->Activate();
	for (int i = 0; i < Spline->GetNumberOfSplinePoints() - 2; ++i)
	{
		USplineMeshComponent* SplineMeshComponent = Cast<USplineMeshComponent>(
			AddComponentByClass(USplineMeshComponent::StaticClass(),
			false,
			FTransform(),
			false));
		SplineMeshComponent->SetMobility(EComponentMobility::Movable);
		if (SplineThrowDrawStaticMesh && SplineThrowDrawMaterial)
		{
			SplineMeshComponent->SetStaticMesh(SplineThrowDrawStaticMesh);
			SplineMeshComponent->SetMaterial(0, SplineThrowDrawMaterial);
		}

		Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		SplineMeshComponent->SetStartAndEnd(
			Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local),
			Spline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local),
			Spline->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local),
			Spline->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Local)
		);

		FTransform Door1Transform = Spline->GetTransformAtSplinePoint(0, ESplineCoordinateSpace::Local, false);
		FRotator Rot=Door1Transform.GetRotation().Rotator();
		Rot.Yaw += 90.f;
		Door1->SetRelativeLocationAndRotation(Door1Transform.GetLocation(), Rot);
		BoxCol1->SetRelativeLocationAndRotation(Door1Transform.GetLocation(), Rot);

		FTransform Door2Transform = Spline->GetTransformAtSplinePoint(Spline->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::Local, false);
		Rot = Door2Transform.GetRotation().Rotator();
		Rot.Yaw += 90.f;
		Door2->SetRelativeLocationAndRotation(Door2Transform.GetLocation(), Rot);
		BoxCol2->SetRelativeLocationAndRotation(Door2Transform.GetLocation(), Rot);
	}
}

void APortal::AddSpline(const FVector& Location)
{
	Spline->AddSplinePoint(Location, ESplineCoordinateSpace::World);
}

void APortal::Box1CollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bPortal && !bReversePortal)
	{
		AMultiPlayCharacter* MultiPlayCharacter = Cast<AMultiPlayCharacter>(OtherActor);
		if (MultiPlayCharacter)
		{
			MultiPlayCharacter->PortalJump(Spline, false);
			bPortal = true;
		}
	}
	if (bReversePortal)
	{
		AMultiPlayCharacter* MultiPlayCharacter = Cast<AMultiPlayCharacter>(OtherActor);
		if (MultiPlayCharacter)
		{
			GetWorldTimerManager().SetTimer(
				ReversePartolTimer,
				this,
				&APortal::ReversePortalTimerFinish,
				1.f
			);
		}
	}
}

void APortal::Box2CollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bPortal && !bReversePortal)
	{
		AMultiPlayCharacter* MultiPlayCharacter = Cast<AMultiPlayCharacter>(OtherActor);
		if (MultiPlayCharacter)
		{
			MultiPlayCharacter->PortalJump(Spline, true);
			bReversePortal = true;
		}
	}
	if (bPortal)
	{
		AMultiPlayCharacter* MultiPlayCharacter = Cast<AMultiPlayCharacter>(OtherActor);
		if (MultiPlayCharacter)
		{
			GetWorldTimerManager().SetTimer(
				PartolTimer,
				this,
				&APortal::PortalTimerFinish,
				1.f
			);
		}
	}
}

void APortal::PortalTimerFinish()
{
	bPortal = false;
}

void APortal::ReversePortalTimerFinish()
{
	bReversePortal = false;
}