
#include "ObjectPoolActor.h"

AObjectPoolActor::AObjectPoolActor()
{
	PrimaryActorTick.bCanEverTick = true;
	MaxLifespan = 0.f;
	bInuse = false;
}

void AObjectPoolActor::BeginPlay()
{
	Super::BeginPlay();
}

void AObjectPoolActor::ObjectPoolInUse(bool Enable)
{
	SetActive(Enable);
	bInuse = Enable;
}

void AObjectPoolActor::SetActive(bool Enable)
{
	if (Enable)
	{
		SetActorTickEnabled(true);
		SetActorHiddenInGame(false);
		SetActorEnableCollision(true);
		TArray<UActorComponent*> Components = GetComponents().Array();
		for (auto Component : Components)
		{
			if (Component)
				Component->Activate(true);
		}
		if (MaxLifespan > 0.0f)
		{
			GetWorldTimerManager().SetTimer(
				LifeSpanTimer, 
				this, 
				&AObjectPoolActor::OnLifeSpanExpired, 
				MaxLifespan);
		}
	}
	else
	{
		SetActorTickEnabled(false);
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		TArray<UActorComponent*> Components = GetComponents().Array();
		for (auto Component : Components)
		{
			if (Component)
				Component->Deactivate();
		}
		GetWorldTimerManager().ClearTimer(LifeSpanTimer);
	}
}

void AObjectPoolActor::OnLifeSpanExpired()
{
	ObjectPoolInUse(false);
}

void AObjectPoolActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

