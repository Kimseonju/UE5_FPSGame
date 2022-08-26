// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAI_BossTargetFireEvent.h"

#include "UE5_MultiPlayFPS/AI/EnemyAIController.h"
#include "UE5_MultiPlayFPS/Character/BossEnemy.h"
#include "UE5_MultiPlayFPS/Character/MultiPlayCharacter.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UEnemyAI_BossTargetFireEvent::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AEnemyAIController* Controller = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	if (Controller == nullptr)  return EBTNodeResult::Type::Failed;
	ABossEnemy* Enemy = Cast<ABossEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (Enemy == nullptr)  return EBTNodeResult::Type::Failed;

	Enemy->FireEvent();

	return EBTNodeResult::Type::Succeeded;
}

void UEnemyAI_BossTargetFireEvent::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
}

EBTNodeResult::Type UEnemyAI_BossTargetFireEvent::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	return Super::AbortTask(OwnerComp, NodeMemory);
}
