// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UE5_MultiPlayFPS/Interface/InteractionNPC_Interface.h"
#include "DialogComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE5_MULTIPLAYFPS_API UDialogComponent : 
	public UActorComponent,
	public IInteractionNPC_Interface
{
	GENERATED_BODY()

public:	
	UDialogComponent();

protected:
	virtual void BeginPlay() override;
	
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
protected:
	class AAIController* AIController;
	class UUserWidget* DialogWidget;
public:

	UFUNCTION(BlueprintCallable)
		void OnInteraction(class AMultiPlayCharacter* Character);
};
