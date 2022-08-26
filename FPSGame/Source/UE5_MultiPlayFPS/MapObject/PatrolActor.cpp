// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolActor.h"
#include "Components/StaticMeshComponent.h"
APatrolActor::APatrolActor()
{
	PrimaryActorTick.bCanEverTick = true;
	Speed = 100.f;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	SetRootComponent(Mesh);
}

void APatrolActor::BeginPlay()
{
	Super::BeginPlay();
	StartPoint = GetActorLocation();
	EndPoint = StartPoint + (PatrolPoint*GetActorScale());
}

// Called every frame
void APatrolActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Location = GetActorLocation();

	float Length = (EndPoint - StartPoint).Size();
	float Travelled = (Location - StartPoint).Size();
	if (Travelled >= Length)
	{
		FVector Temp = StartPoint;
		StartPoint = EndPoint;
		EndPoint = Temp;
	}

	FVector Direction = (EndPoint - StartPoint).GetSafeNormal();
	Location += Speed* DeltaTime * Direction;
	SetActorLocation(Location);
}

