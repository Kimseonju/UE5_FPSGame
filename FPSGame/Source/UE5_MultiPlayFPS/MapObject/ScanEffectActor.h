// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ScanEffectActor.generated.h"

UCLASS()
class UE5_MULTIPLAYFPS_API AScanEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AScanEffectActor();

protected:
	virtual void BeginPlay() override;
private:

	UPROPERTY(VisibleAnywhere,  meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* EffectMesh;
	UPROPERTY(EditAnywhere,  meta = (AllowPrivateAccess = "true"))
	float LifeTime;
public:	
	virtual void Tick(float DeltaTime) override;

};
