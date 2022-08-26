// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAI_BossDash.h"


#include "UE5_MultiPlayFPS/AI/EnemyAIController.h"
#include "UE5_MultiPlayFPS/Character/BossEnemy.h"
#include "UE5_MultiPlayFPS/Character/MultiPlayCharacter.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
UEnemyAI_BossDash::UEnemyAI_BossDash()
{
}

EBTNodeResult::Type UEnemyAI_BossDash::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
		Enemy->Dash();
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(Controller, pPlayer->GetActorLocation());
	}

	return EBTNodeResult::Type::Succeeded;
}

void UEnemyAI_BossDash::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
}

EBTNodeResult::Type UEnemyAI_BossDash::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	return Super::AbortTask(OwnerComp, NodeMemory);
}
