// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAI_BossMeleeAttack.h"

#include "UE5_MultiPlayFPS/AI/EnemyAIController.h"
#include "UE5_MultiPlayFPS/Character/BossEnemy.h"
#include "UE5_MultiPlayFPS/Character/MultiPlayCharacter.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
UEnemyAI_BossMeleeAttack::UEnemyAI_BossMeleeAttack()
{
}

EBTNodeResult::Type UEnemyAI_BossMeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AEnemyAIController* Controller = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	if (Controller == nullptr)  return EBTNodeResult::Type::Failed;
	ABossEnemy* Enemy = Cast<ABossEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (Enemy == nullptr)  return EBTNodeResult::Type::Failed;



	AMultiPlayCharacter* pPlayer = Cast<AMultiPlayCharacter>(Controller->GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));

	Enemy->PlayAttackMontage("AttackCombo1", 1.f);
	Controller->StopMovement();

	return EBTNodeResult::Type::Succeeded;
}

void UEnemyAI_BossMeleeAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
}

EBTNodeResult::Type UEnemyAI_BossMeleeAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	return Super::AbortTask(OwnerComp, NodeMemory);
}
