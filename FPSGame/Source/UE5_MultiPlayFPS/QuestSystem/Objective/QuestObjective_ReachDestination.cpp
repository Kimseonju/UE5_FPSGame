// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestObjective_ReachDestination.h"
#include "UE5_MultiPlayFPS/Character/MultiPlayCharacter.h"
#include "Engine/TriggerBox.h"
#include "UE5_MultiPlayFPS/Widget/HUD/CharacterOverlay.h"
#include "UE5_MultiPlayFPS/PlayerController/MultiPlayPlayerController.h"
#include "UE5_MultiPlayFPS/MapObject/QuestTriggerBox.h"
void UQuestObjective_ReachDestination::BeginTrigger(AActor* _TriggerActor, AActor* _OtherActor)
{
	AMultiPlayCharacter* Character=Cast<AMultiPlayCharacter>(_OtherActor);
	if (Character)
	{
		TriggerBox->OnActorBeginOverlap.RemoveDynamic(this, &UQuestObjective_ReachDestination::BeginTrigger);

		FString EventText = Name.ToString();
		EventText = EventText + " Complete";

		PlayerController->GetCharacterOverlay()->QuestTextEvent(FText::FromString(EventText));
		QuestClear = true;
		OnCompletedDelegate.Broadcast(this);
	}
}

void UQuestObjective_ReachDestination::ActivateObjective()
{
	Super::ActivateObjective();
	if (TriggerBox)
	{
		TriggerBox->OnActorBeginOverlap.AddDynamic(this, &UQuestObjective_ReachDestination::BeginTrigger);
		AQuestTriggerBox* QuestTriggerBox=Cast< AQuestTriggerBox>(TriggerBox);
		if (QuestTriggerBox)
			QuestTriggerBox->QuestOn();
	}

}

void UQuestObjective_ReachDestination::PrevInit(class AMultiPlayPlayerController* Controller, FName ObjectName, class ATriggerBox* TriggerBox_)
{
	PlayerController = Controller;
	Name = ObjectName;
	TriggerBox = TriggerBox_;
}
