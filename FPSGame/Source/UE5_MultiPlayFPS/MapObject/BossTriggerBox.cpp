// Fill out your copyright notice in the Description page of Project Settings.


#include "BossTriggerBox.h"
#include "LevelSequence\Public\LevelSequence.h"
#include "LevelSequence\Public\LevelSequencePlayer.h"
#include "LevelSequence\Public\LevelSequenceActor.h"

ABossTriggerBox::ABossTriggerBox()
{

}

void ABossTriggerBox::BeginPlay()
{

}

void ABossTriggerBox::BeginTrigger(AActor* _TriggerActor, AActor* _OtherActor)
{

	m_LvSeqPlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
		GetWorld(), m_LvSeq, FMovieSceneSequencePlaybackSettings(), m_LvSeqActor);

	m_LvSeqPlayer->Play();
	Destroy();
}

void ABossTriggerBox::EndTrigger(AActor* _TriggerActor, AActor* _OtherActor)
{

}
