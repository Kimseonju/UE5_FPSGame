// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AbilityComponent.generated.h"

class UUserWidget;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE5_MULTIPLAYFPS_API UAbilityComponent : public UActorComponent
{
	GENERATED_BODY()
	friend class AMultiPlayCharacter;

public:	
	UAbilityComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void CreateScanEnemyWidget(const FHitResult& HitResult);
	void StoreScanEnemy(UScanEnemyWidget* ScanEnemy, FVector Location);
	UFUNCTION()
	void DestroyScanEnemy(UScanEnemyWidget* ScanEnemy);
	void UpdateScanEnemys();
private:
	UPROPERTY(EditAnywhere)
	float ScanTimer;
	UPROPERTY(EditAnywhere)
	float ScanTimerMax;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class AScanEffectActor> ScanEffectActor;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UScanEnemyWidget> ScanEffectUMGclass;
	UPROPERTY(VisibleAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
		TMap<UScanEnemyWidget*, FVector> WidgetScanEnemys;

	UPROPERTY()
	class AMultiPlayCharacter* Character;
	UPROPERTY()
	class AMultiPlayPlayerController* Controller;

	UPROPERTY(EditAnywhere)
		float ScanEnemyDestroyTime;
	

	float CreatePortalTimer;
	UPROPERTY(EditAnywhere)
	float CreatePortalTimerMax;
	UPROPERTY(EditAnywhere, Category="Portal")
	TSubclassOf<class APortal> PortalClass;
	class APortal* Portal;
	FTimerHandle PortalTimer;
	UPROPERTY(EditAnywhere, Category = "Portal")
	UTexture2D* PortalImage;

private:
	void AddPortalSplinePoint();
public:	
	void ScanEnemy();
	void CreatePortal();
	float GetScanTimer() const { return ScanTimer; }
	float GetScanTimerMax()const { return ScanTimerMax; }
	float GetCreatePortalTimer() const { return CreatePortalTimer; }
	float GetCreatePortalTimerMax()const { return CreatePortalTimerMax; }
public:
	UTexture2D* GetPortalImage()
	{
		return PortalImage;
	}
};
