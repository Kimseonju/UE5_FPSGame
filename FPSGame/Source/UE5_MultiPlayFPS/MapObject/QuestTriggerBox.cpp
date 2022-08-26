// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestTriggerBox.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SceneComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/ShapeComponent.h"
AQuestTriggerBox::AQuestTriggerBox()
{
	RootComp = CreateDefaultSubobject<USceneComponent>("RootComp");
	SetRootComponent(RootComp);
	GetCollisionComponent()->SetupAttachment(RootComp);
	GetSpriteComponent()->bHiddenInGame = true;
	ParticleComponent =CreateDefaultSubobject<UParticleSystemComponent>("ParticleSystemComponent");
	ParticleComponent->SetupAttachment(RootComp);
}

void AQuestTriggerBox::QuestOn()
{
	ParticleComponent->SetVisibility(true);
}

void AQuestTriggerBox::QuestOff()
{
	ParticleComponent->SetVisibility(false);
}

void AQuestTriggerBox::BeginPlay()
{
	OnActorBeginOverlap.AddDynamic(this, &AQuestTriggerBox::BeginTrigger);
	ParticleComponent->SetVisibility(false);
}

void AQuestTriggerBox::BeginTrigger(AActor* _TriggerActor, AActor* _OtherActor)
{
	ParticleComponent->SetVisibility(false);
}

void AQuestTriggerBox::EndTrigger(AActor* _TriggerActor, AActor* _OtherActor)
{

}
