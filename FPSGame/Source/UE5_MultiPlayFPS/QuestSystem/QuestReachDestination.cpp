
#include "QuestReachDestination.h"

#include "UE5_MultiPlayFPS/QuestSystem/Objective/QuestObjectiveCollection.h"
#include "UE5_MultiPlayFPS/Character/MultiPlayCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "UE5_MultiPlayFPS/FieldManager.h"
#include "UE5_MultiPlayFPS/QuestSystem/Objective/QuestObjective_ReachDestination.h"
#include "UE5_MultiPlayFPS/Widget/HUD/CharacterOverlay.h"
#include "UE5_MultiPlayFPS/PlayerController/MultiPlayPlayerController.h"

class UQuestObjectiveCollection* AQuestReachDestination::ConstructRootObjectiveCollection()
{

	AMultiPlayCharacter* Character = Cast<AMultiPlayCharacter>(GetOwner());
	if (Character)
	{
		UQuestObjectiveCollection* Collection = NewObject<UQuestObjectiveCollection>(UQuestObjectiveCollection::StaticClass());

		Collection->PrevInit(Character->GetMultiPlayPlayerController(), true, FName("Boom"));
		return Collection;
	}
	return nullptr;
}

void AQuestReachDestination::PopulateObjective(class UQuestObjectiveCollection* ObjectiveCollection)
{
	{
		if (TriggerBox1)
		{
			UQuestObjective_ReachDestination* QuestReachDestination = NewObject<UQuestObjective_ReachDestination>(UQuestObjective_ReachDestination::StaticClass());
			QuestReachDestination->PrevInit(ObjectiveCollection->GetPlayerController(), TriggerName1, TriggerBox1);
			ObjectiveCollection->AddObjective(QuestReachDestination);
		}
	}
	{
		if (TriggerBox2)
		{
			UQuestObjective_ReachDestination* QuestReachDestination = NewObject<UQuestObjective_ReachDestination>(UQuestObjective_ReachDestination::StaticClass());
			QuestReachDestination->PrevInit(ObjectiveCollection->GetPlayerController(), TriggerName2, TriggerBox2);
			ObjectiveCollection->AddObjective(QuestReachDestination);
		}
	}
	{
		if (TriggerBox3)
		{
			UQuestObjective_ReachDestination* QuestReachDestination = NewObject<UQuestObjective_ReachDestination>(UQuestObjective_ReachDestination::StaticClass());
			QuestReachDestination->PrevInit(ObjectiveCollection->GetPlayerController(), TriggerName3, TriggerBox3);
			ObjectiveCollection->AddObjective(QuestReachDestination);
		}
	}
	FString str = TEXT("QuestBoom");
	FText Text = FText::FromString(str);
	ObjectiveCollection->GetPlayerController()->GetCharacterOverlay()->QuestTextEvent(Text);
}

void AQuestReachDestination::PrevInit(class ATriggerBox* Box1, class ATriggerBox* Box2, class ATriggerBox* Box3, FName Name1, FName Name2, FName Name3)
{
	TriggerBox1 = Box1;
	TriggerBox2 = Box2;
	TriggerBox3 = Box3;

	TriggerName1 = Name1;
	TriggerName2 = Name2;
	TriggerName3 = Name3;
}
