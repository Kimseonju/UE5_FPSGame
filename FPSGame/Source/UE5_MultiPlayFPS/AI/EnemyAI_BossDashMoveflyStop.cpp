
#include "EnemyAI_BossDashMoveflyStop.h"

#include "UE5_MultiPlayFPS/AI/EnemyAIController.h"
#include "UE5_MultiPlayFPS/Character/BossEnemy.h"
#include "UE5_MultiPlayFPS/Character/MultiPlayCharacter.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UEnemyAI_BossDashMoveflyStop::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AEnemyAIController* Controller = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	if (Controller == nullptr)  return EBTNodeResult::Type::Failed;
	ABossEnemy* Enemy = Cast<ABossEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (Enemy == nullptr)  return EBTNodeResult::Type::Failed;

	Enemy->DashMove_FlyEnd();

	return EBTNodeResult::Type::Succeeded;
}

void UEnemyAI_BossDashMoveflyStop::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
}

EBTNodeResult::Type UEnemyAI_BossDashMoveflyStop::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	return Super::AbortTask(OwnerComp, NodeMemory);
}
