// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "EnemyAI_BossPullTarget.generated.h"

/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API UEnemyAI_BossPullTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UEnemyAI_BossPullTarget();

protected:
	// �۾� ����
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// �۾� ���� �� �� ������ ȣ��
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	// �۾� Ż��
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
