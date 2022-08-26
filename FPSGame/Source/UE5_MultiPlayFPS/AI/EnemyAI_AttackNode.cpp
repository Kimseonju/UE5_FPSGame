// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAI_AttackNode.h"
#include "UE5_MultiPlayFPS/AI/EnemyAIController.h"
#include "UE5_MultiPlayFPS/Character/Enemy.h"
UEnemyAI_AttackNode::UEnemyAI_AttackNode()
{
}

EBTNodeResult::Type UEnemyAI_AttackNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AEnemyAIController* Controller = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	if (Controller == nullptr)  return EBTNodeResult::Type::Failed;
	AEnemy* Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (Enemy == nullptr)  return EBTNodeResult::Type::Failed;
	Enemy->PlayAttackMontage(Enemy->GetAttackSectionName(), 1.f);
	return EBTNodeResult::Type::Succeeded;
}

void UEnemyAI_AttackNode::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
}

EBTNodeResult::Type UEnemyAI_AttackNode::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	return Super::AbortTask(OwnerComp, NodeMemory);
}
