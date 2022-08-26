// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "EnemySpawnTriggerBox.generated.h"

class AEnemy;
/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API AEnemySpawnTriggerBox : public ATriggerBox
{
	GENERATED_BODY()
public:
	AEnemySpawnTriggerBox();

protected:
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere,  meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AEnemy> EnemyClass;
	UPROPERTY(EditAnywhere,  meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AEnemy> GunEnemyClass;
	UPROPERTY(EditAnywhere,  meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AEnemy> NotMoveEnemyClass;
	UPROPERTY(EditAnywhere,  meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AEnemy> GoPlayerEnemyClass;
	UPROPERTY(EditAnywhere,  meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AEnemy> SniperEnemyClass;

	UPROPERTY(EditAnywhere,  meta=(AllowPrivateAccess="true", MakeEditWidget = "true"))
	TArray<FTransform> EnemySpawn;
	UPROPERTY(EditAnywhere,  meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	TArray<FTransform> GunEnemySpawn;
	UPROPERTY(EditAnywhere,  meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	TArray<FTransform> NotMoveEnemySpawn;
	UPROPERTY(EditAnywhere,  meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	TArray<FTransform> GoPlayerEnemySpawn;
	UPROPERTY(EditAnywhere,  meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	TArray<FTransform> SniperEnemySpawn;

	UPROPERTY(EditAnywhere,  meta = (AllowPrivateAccess = "true"))
	bool bDestroy;
protected:
	UFUNCTION()
	void BeginTrigger(AActor* _TriggerActor, AActor* _OtherActor);

};
