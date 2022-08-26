// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAI_BossFieldSkill.h"

#include "UE5_MultiPlayFPS/AI/EnemyAIController.h"
#include "UE5_MultiPlayFPS/Character/BossEnemy.h"
#include "UE5_MultiPlayFPS/Character/MultiPlayCharacter.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
UEnemyAI_BossFieldSkill::UEnemyAI_BossFieldSkill()
{
}

EBTNodeResult::Type UEnemyAI_BossFieldSkill::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AEnemyAIController* Controller = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	if (Controller == nullptr)  return EBTNodeResult::Type::Failed;
	ABossEnemy* Enemy = Cast<ABossEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (Enemy == nullptr)  return EBTNodeResult::Type::Failed;



	AMultiPlayCharacter* pPlayer = Cast<AMultiPlayCharacter>(Controller->GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));

	// 플레이어가 탐지가 되었다면
	if (nullptr != pPlayer)
	{
		Enemy->BossAttack(BossPattern::EP_FieldSkill);
		Controller->StopMovement();
	}

	return EBTNodeResult::Type::Succeeded;
}

void UEnemyAI_BossFieldSkill::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
}

EBTNodeResult::Type UEnemyAI_BossFieldSkill::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	return Super::AbortTask(OwnerComp, NodeMemory);
}
