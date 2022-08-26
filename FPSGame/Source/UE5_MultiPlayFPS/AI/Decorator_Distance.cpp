// Fill out your copyright notice in the Description page of Project Settings.


#include "Decorator_Distance.h"
#include "UE5_MultiPlayFPS/AI/EnemyAIController.h"
#include "UE5_MultiPlayFPS/Character/MultiPlayCharacter.h"
#include "UE5_MultiPlayFPS/Character/Enemy.h"
#include "BehaviorTree/BlackboardComponent.h"
bool UDecorator_Distance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{

	AEnemyAIController* Controller = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	AEnemy* Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (Enemy == nullptr || Controller == nullptr) return false;

	AMultiPlayCharacter* Target = Cast<AMultiPlayCharacter>(Controller->GetBlackboardComponent()->GetValueAsObject("Target"));
	if (Target == nullptr) return false;

	float Length = (Enemy->GetActorLocation() - Target->GetActorLocation()).Size();
	return Distance <= Length;
}
