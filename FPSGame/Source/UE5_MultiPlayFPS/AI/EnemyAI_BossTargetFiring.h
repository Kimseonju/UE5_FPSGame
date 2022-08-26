// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "EnemyAI_BossTargetFiring.generated.h"

/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API UEnemyAI_BossTargetFiring : public UBTTaskNode
{
	GENERATED_BODY()
private:
	int FireIndex = 0;
	bool bFireEnd;
	class ABossEnemy* Enemy;
protected:
	// �۾� ����
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// �۾� ���� �� �� ������ ȣ��
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	// �۾� Ż��
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	void Fire();
};