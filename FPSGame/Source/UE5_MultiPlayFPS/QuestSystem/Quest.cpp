
#include "Quest.h"
#include "UE5_MultiPlayFPS/QuestSystem/Objective/QuestObjectiveCollection.h"
#include "UE5_MultiPlayFPS/Character/NPC.h"
AQuest::AQuest()
{
	PrimaryActorTick.bCanEverTick = false;
	
}

void AQuest::BeginPlay()
{
	Super::BeginPlay();

}

void AQuest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AQuest::PostInit()
{

	RootObjectiveCollection = ConstructRootObjectiveCollection();
	
	if (RootObjectiveCollection)
	{
		ANPC* NPC=Cast<ANPC>(GetInstigator());
		if (NPC)
			RootObjectiveCollection->SetNPC(NPC);
		PopulateObjective(RootObjectiveCollection);
		RootObjectiveCollection->ActivateObjective();
	}
}

void AQuest::PopulateObjective(UQuestObjectiveCollection* ObjectiveCollection)
{

}

bool AQuest::GetIsComplete()
{
	return RootObjectiveCollection->GetIsComplete();
}


