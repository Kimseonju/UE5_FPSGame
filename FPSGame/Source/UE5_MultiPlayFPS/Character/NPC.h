// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NPC.generated.h"

UCLASS()
class UE5_MULTIPLAYFPS_API ANPC : 
	public ACharacter
{
	GENERATED_BODY()

public:
	ANPC();
private:
	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent* ParticleComponent;

protected:
	virtual void BeginPlay() override;
	
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
public:
	UFUNCTION(BlueprintCallable)
	void QuestOn();
	UFUNCTION(BlueprintCallable)
	void QuestOff();

};
