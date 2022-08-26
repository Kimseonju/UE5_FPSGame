// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestObjectiveCollection.h"
#include "UE5_MultiPlayFPS/Character/NPC.h"
UQuestObjective* UQuestObjectiveCollection::FindNextInCompleteObject()
{
	for (UQuestObjective* Objective : Objectives)
	{
		if (!Objective->GetIsComplete())
			return Objective;
		
	}
	return nullptr;
}

void UQuestObjectiveCollection::OnCompletedBind(UQuestObjective* Objective)
{
	Objective->OnCompletedDelegate.RemoveDynamic(this, &UQuestObjectiveCollection::OnCompletedBind);

	UQuestObjective* NextObjective = FindNextInCompleteObject();
	if (NextObjective)
	{
		NextObjective->ActivateObjective();
		NextObjective->OnCompletedDelegate.AddDynamic(this, &UQuestObjectiveCollection::OnCompletedBind);
	}
	else
	{
		if (QuestNPC)
			QuestNPC->QuestOn();
	}
}

void UQuestObjectiveCollection::ActivateObjective()
{
	Super::ActivateObjective();
	if (bOrderRequired)
	{
		OnCompletedDelegate.RemoveDynamic(this, &UQuestObjectiveCollection::OnCompletedBind);
		UQuestObjective* Objective = FindNextInCompleteObject();
		if (Objective)
		{
			Objective->ActivateObjective();
			Objective->OnCompletedDelegate.AddDynamic(this, &UQuestObjectiveCollection::OnCompletedBind);
		}
	}
}

void UQuestObjectiveCollection::PrevInit(class AMultiPlayPlayerController* Controller, bool OrderRequired, FName ObjectName)
{
	PlayerController = Controller;
	bOrderRequired = OrderRequired;
	Name = ObjectName;
}
