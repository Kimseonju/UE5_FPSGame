// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectPoolActor.generated.h"

UCLASS()
class UE5_MULTIPLAYFPS_API AObjectPoolActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AObjectPoolActor();
	UFUNCTION(BlueprintCallable)
	virtual void ObjectPoolInUse(bool Enable);

protected:
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere)
	bool bInuse;
	UPROPERTY(VisibleAnywhere)
	float ObjectPoolLifeTime;
	UPROPERTY(EditAnywhere)
	float MaxLifespan;
private:
	FTimerHandle LifeSpanTimer;
protected:
	void SetActive(bool Enable);
	void OnLifeSpanExpired();
public:	
	void SetLifeSpan(float LifeSpan) { MaxLifespan = LifeSpan; }
	virtual void Tick(float DeltaTime) override;
	bool IsUse()const { return bInuse; }
};
