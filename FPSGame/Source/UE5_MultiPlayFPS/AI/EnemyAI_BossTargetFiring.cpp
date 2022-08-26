// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAI_BossTargetFiring.h"

#include "UE5_MultiPlayFPS/AI/EnemyAIController.h"
#include "UE5_MultiPlayFPS/Character/BossEnemy.h"
#include "UE5_MultiPlayFPS/Character/MultiPlayCharacter.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UEnemyAI_BossTargetFiring::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	bNotifyTick = 1;
	bFireEnd = false;
	FireIndex = 0.f;
	AEnemyAIController* Controller = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	if (Controller == nullptr)  return EBTNodeResult::Type::Failed;
	Enemy = Cast<ABossEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (Enemy == nullptr)  return EBTNodeResult::Type::Failed;
	Fire();
	return EBTNodeResult::Type::InProgress;
}
void UEnemyAI_BossTargetFiring::TickTask(UBehaviorTreeComponent& OwnerComp, 
	uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (bFireEnd)
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}
void UEnemyAI_BossTargetFiring::Fire()
{
	FTimerHandle WaitHandle;
	if (FireIndex >= 100.f)
	{
		bFireEnd = true;
		return;
	}
	GetWorld()->GetTimerManager().SetTimer(
		WaitHandle, 
		FTimerDelegate::CreateLambda([&]()
		{
			Enemy->Firing();
			Fire();
		}),
		0.05f, false
	);
	FireIndex++;
	return;
}

EBTNodeResult::Type UEnemyAI_BossTargetFiring::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return Super::AbortTask(OwnerComp, NodeMemory);
}