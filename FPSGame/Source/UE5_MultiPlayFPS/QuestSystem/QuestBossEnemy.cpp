#include "QuestBossEnemy.h"
#include "UE5_MultiPlayFPS/QuestSystem/Objective/QuestObjectiveCollection.h"
#include "UE5_MultiPlayFPS/Character/MultiPlayCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "UE5_MultiPlayFPS/FieldManager.h"
#include "UE5_MultiPlayFPS/QuestSystem/Objective/QuestObjective_EnemyKill.h"
#include "UE5_MultiPlayFPS/Widget/HUD/CharacterOverlay.h"
#include "UE5_MultiPlayFPS/PlayerController/MultiPlayPlayerController.h"

UQuestObjectiveCollection* AQuestBossEnemy::ConstructRootObjectiveCollection()
{
	AMultiPlayCharacter* Character = Cast<AMultiPlayCharacter>(GetOwner());
	if (Character)
	{
		UQuestObjectiveCollection* Collection = NewObject<UQuestObjectiveCollection>(UQuestObjectiveCollection::StaticClass());
		Collection->PrevInit(Character->GetMultiPlayPlayerController(), true, "BossKill");
		return Collection;
	}
	return nullptr;
}

void AQuestBossEnemy::PopulateObjective(class UQuestObjectiveCollection* ObjectiveCollection)
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFieldManager::StaticClass(), Actors);

	if (!Actors.IsEmpty())
	{
		UQuestObjective_EnemyKill* QuestEnemyKill = NewObject<UQuestObjective_EnemyKill>(UQuestObjective_EnemyKill::StaticClass());
		QuestEnemyKill->PrevInit(ObjectiveCollection->GetPlayerController(), "BossKill", EnemyKill, (AFieldManager*)Actors[0]);
		ObjectiveCollection->AddObjective(QuestEnemyKill);

		FString str = TEXT("Quest_BossKill");
		FText Text = FText::FromString(str);

		ObjectiveCollection->GetPlayerController()->GetCharacterOverlay()->QuestTextEvent(Text);
	}
}

void AQuestBossEnemy::PrevInit(int EnemyKillMax)
{
	EnemyKill = EnemyKillMax;
}
