// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolActor.generated.h"

UCLASS()
class UE5_MULTIPLAYFPS_API APatrolActor : public AActor
{
	GENERATED_BODY()
	
public:	
	APatrolActor();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Mesh;

	FVector StartPoint;
	FVector EndPoint;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector PatrolPoint;


	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float Speed;
	int MovePoint;
public:	

};
