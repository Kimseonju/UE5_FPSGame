// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Quest.generated.h"
class UQuestObjectiveCollection;
UCLASS()
class UE5_MULTIPLAYFPS_API AQuest : public AActor
{
	GENERATED_BODY()
	
public:	
	AQuest();
protected:
	virtual void BeginPlay() override;
	
public:	
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UQuestObjectiveCollection* RootObjectiveCollection;
protected:
	virtual UQuestObjectiveCollection* ConstructRootObjectiveCollection() { return RootObjectiveCollection; }
	UFUNCTION(BlueprintCallable)
	void PostInit();
public:
	virtual void PopulateObjective(class UQuestObjectiveCollection* ObjectiveCollection);
	UFUNCTION(BlueprintCallable)
	virtual class UQuestObjectiveCollection* GetRootObjectiveCollection() { return RootObjectiveCollection; }
	UFUNCTION(BlueprintCallable)
	virtual bool GetIsComplete();

};
