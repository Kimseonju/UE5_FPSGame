// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "Service_SelectTargetActor.generated.h"

/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API UService_SelectTargetActor : public UBTService
{
	GENERATED_BODY()

public:
	UService_SelectTargetActor();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
private:
	UPROPERTY()
	class UBlackboardComponent* BlackboardComponent;
	
};
