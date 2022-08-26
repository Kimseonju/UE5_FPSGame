// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectPoolComponent.h"
#include "UE5_MultiPlayFPS/Performance/ObjectPoolActor.h"
UObjectPoolComponent::UObjectPoolComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UObjectPoolComponent::BeginPlay()
{
	Super::BeginPlay();
	if (ObjectPoolActorClass)
	{
		FActorSpawnParameters Param;
		for (int i = 0; i < PoolSize; i++)
		{
			AObjectPoolActor* PoolActor = GetWorld()->SpawnActor<AObjectPoolActor>(
				ObjectPoolActorClass, 
				FVector().ZeroVector, 
				FRotator().ZeroRotator
				);
			if (PoolActor)
			{
				PoolActor->ObjectPoolInUse(false);
				PoolActors.Add(PoolActor);
			}
		}
	}
}

AObjectPoolActor* UObjectPoolComponent::SpawnPoolActor()
{
	for (AObjectPoolActor* PoolActor : PoolActors)
	{
		if (PoolActor && !PoolActor->IsUse())
		{
			PoolActor->ObjectPoolInUse(true);
			return PoolActor;
		}
	}
	if (ObjectPoolActorClass)
	{
		AObjectPoolActor* PoolActor = GetWorld()->SpawnActor<AObjectPoolActor>(
			ObjectPoolActorClass, 
			FVector().ZeroVector, 
			FRotator().ZeroRotator);
		if (PoolActor)
		{
			PoolActor->ObjectPoolInUse(true);
			PoolActor->SetOwner(GetOwner());
			PoolActor->SetInstigator(Cast<APawn>(GetOwner()));
			PoolActors.Add(PoolActor);
			return PoolActor;
		}
	}
	return nullptr;
}

void UObjectPoolComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	PoolSaveSize = PoolActors.Num();
}

