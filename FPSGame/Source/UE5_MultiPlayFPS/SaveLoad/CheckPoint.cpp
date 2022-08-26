// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckPoint.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "UE5_MultiPlayFPS/Character/MultiPlayCharacter.h"
#include "SaveLoadManager.h"
#include "UE5_MultiPlayFPS/PlayerState/MultiPlayPlayerState.h"
ACheckPoint::ACheckPoint()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>("RootComp");
	SetRootComponent(RootComp);

	Box = CreateDefaultSubobject<UBoxComponent>("Box");
	Box->SetupAttachment(RootComp);

}

void ACheckPoint::BeginPlay()
{
	Super::BeginPlay();
	Box->OnComponentBeginOverlap.AddDynamic(this, &ACheckPoint::CollisionBeginOverlap);
	
}

void ACheckPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACheckPoint::CollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMultiPlayCharacter* Character = Cast<AMultiPlayCharacter>(OtherActor);
	if (Character)
	{
		if (SaveLoadManager)
		{
			SaveLoadManager->SaveGame();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("SaveLoadManager Is Nullptr"));
		}
	}
}

