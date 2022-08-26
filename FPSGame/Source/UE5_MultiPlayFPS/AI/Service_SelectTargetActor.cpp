// Fill out your copyright notice in the Description page of Project Settings.


#include "Service_SelectTargetActor.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "UE5_MultiPlayFPS/AI/EnemyAIController.h"
#include "UE5_MultiPlayFPS/Character/MultiPlayCharacter.h"
#include "UE5_MultiPlayFPS/Character/Enemy.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
UService_SelectTargetActor::UService_SelectTargetActor()
{

}

void UService_SelectTargetActor::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* pController = OwnerComp.GetAIOwner();
	if (nullptr == pController)
		return;

	AEnemy* Enemy = Cast<AEnemy>(pController->GetPawn());

	UAIPerceptionComponent * AIPerceptionComponent= Cast<UAIPerceptionComponent>(Enemy->GetComponentByClass(UAIPerceptionComponent::StaticClass()));
	if (AIPerceptionComponent)
	{
		TArray<AActor*> arrActor;
		AIPerceptionComponent->GetKnownPerceivedActors(UAISense_Sight::StaticClass(), arrActor);
		for (AActor* actor : arrActor)
		{
			AMultiPlayCharacter* Player=Cast<AMultiPlayCharacter>(actor);
			if (Player)
			{
				pController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), Player);
				return;
			}
		}
	}
}
