// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FieldManager.generated.h"

UCLASS()
class UE5_MULTIPLAYFPS_API AFieldManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AFieldManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
public:
	TArray<AActor*> GetAllEnemy();
};
