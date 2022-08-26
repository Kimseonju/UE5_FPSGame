// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawnTriggerBox.h"
#include "UE5_MultiPlayFPS/Character/Enemy.h"
AEnemySpawnTriggerBox::AEnemySpawnTriggerBox()
{
	bDestroy = false;
}

void AEnemySpawnTriggerBox::BeginPlay()
{

	OnActorBeginOverlap.AddDynamic(this, &AEnemySpawnTriggerBox::BeginTrigger);
}

void AEnemySpawnTriggerBox::BeginTrigger(AActor* _TriggerActor, AActor* _OtherActor)
{
	for (int i = 0; i < EnemySpawn.Num(); ++i)
	{
		GetWorld()->SpawnActor<AEnemy>(EnemyClass, EnemySpawn[i] + GetActorTransform());
	}
	for (int i = 0; i < GunEnemySpawn.Num(); ++i)
	{
		GetWorld()->SpawnActor<AEnemy>(GunEnemyClass, GunEnemySpawn[i] + GetActorTransform());
	}
	for (int i = 0; i < NotMoveEnemySpawn.Num(); ++i)
	{
		GetWorld()->SpawnActor<AEnemy>(NotMoveEnemyClass, NotMoveEnemySpawn[i] + GetActorTransform());
	}
	for (int i = 0; i < GoPlayerEnemySpawn.Num(); ++i)
	{
		GetWorld()->SpawnActor<AEnemy>(GoPlayerEnemyClass, GoPlayerEnemySpawn[i]+GetActorTransform());
	}
	for (int i = 0; i < SniperEnemySpawn.Num(); ++i)
	{
		GetWorld()->SpawnActor<AEnemy>(SniperEnemyClass, SniperEnemySpawn[i] + GetActorTransform());
	}
	if (bDestroy)
	{
		Destroy();
	}
}

