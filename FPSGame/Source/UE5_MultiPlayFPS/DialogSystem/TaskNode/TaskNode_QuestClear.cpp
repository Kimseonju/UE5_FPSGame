// Fill out your copyright notice in the Description page of Project Settings.


#include "TaskNode_QuestClear.h"

UTaskNode_QuestClear::UTaskNode_QuestClear()
{

}

EBTNodeResult::Type UTaskNode_QuestClear::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	return EBTNodeResult::Type::Succeeded;
}

void UTaskNode_QuestClear::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{

}

EBTNodeResult::Type UTaskNode_QuestClear::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return Super::AbortTask(OwnerComp, NodeMemory);
}
