// Fill out your copyright notice in the Description page of Project Settings.


#include "ScanEffectActor.h"
#include "Components/StaticMeshComponent.h"
// Sets default values
AScanEffectActor::AScanEffectActor()
{
	PrimaryActorTick.bCanEverTick = true;
	EffectMesh = CreateDefaultSubobject<UStaticMeshComponent>("EffectMesh");
	SetRootComponent(EffectMesh);
	LifeTime = 2.f;
}

// Called when the game starts or when spawned
void AScanEffectActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AScanEffectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	LifeTime -= DeltaTime;
	if (LifeTime <= 0.f)
	{
		Destroy();
	}
	EffectMesh->SetRelativeScale3D(EffectMesh->GetRelativeScale3D() + (EffectMesh->GetRelativeScale3D() * DeltaTime*3.f));
}

