// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityComponent.h"
#include "UE5_MultiPlayFPS/Character/Enemy.h"
#include "UE5_MultiPlayFPS/Character/MultiPlayCharacter.h"
#include "UE5_MultiPlayFPS/PlayerController/MultiPlayPlayerController.h"
#include "UE5_MultiPlayFPS/MapObject/ScanEffectActor.h"
#include "UE5_MultiPlayFPS/MapObject/Portal.h"
#include "UE5_MultiPlayFPS/Widget/HUD/ScanEnemyWidget.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
UAbilityComponent::UAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	ScanEnemyDestroyTime = 2.f;
	CreatePortalTimerMax = 2.f;
}

void UAbilityComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


void UAbilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	ScanTimer -= DeltaTime;
	CreatePortalTimer -= DeltaTime;

	if (Controller)
	{
		Controller->SetHUDSkill(this);
	}
	UpdateScanEnemys();
}

void UAbilityComponent::CreateScanEnemyWidget(const FHitResult& HitResult)
{
	if (ScanEffectUMGclass)
	{
		UScanEnemyWidget* ScanEnemyWidget = 
			CreateWidget<UScanEnemyWidget>(GetWorld(), ScanEffectUMGclass);

		ScanEnemyWidget->AddToViewport();
		FVector Location = HitResult.GetActor()->GetActorLocation();
		FVector2D ScreenPosition;
		UGameplayStatics::ProjectWorldToScreen(
			GetWorld()->GetFirstPlayerController(),
			Location,
			ScreenPosition
		);
		FVector2D WidgetSize=ScanEnemyWidget->GetWidgetSize();
		ScanEnemyWidget->SetPositionInViewport(ScreenPosition);
		StoreScanEnemy(ScanEnemyWidget, Location);
	}
}


void UAbilityComponent::ScanEnemy()
{
	if (ScanTimer > 0.f) return;
	ScanTimer = ScanTimerMax;

	FVector vPos = Character->GetActorLocation();
	float fRadius = 1000.f;
	TArray<FHitResult> ScanEnemys;

	GetWorld()->SweepMultiByChannel(ScanEnemys,
		vPos,
		vPos,
		FQuat::Identity,
		ECollisionChannel::ECC_Visibility,
		FCollisionShape::MakeSphere(fRadius)
		);
	for (FHitResult ScanEnemy : ScanEnemys)
	{
		AEnemy* Enemy=Cast<AEnemy>(ScanEnemy.GetActor());
		if (Enemy == nullptr) continue;
		Enemy->Scanning();
		CreateScanEnemyWidget(ScanEnemy);
	}
#pragma region ScanEffectActor
	if (ScanEffectActor)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters ActorSpawnParams;
			World->SpawnActor<AScanEffectActor>(
				ScanEffectActor,
				Character->GetActorLocation(),
				FRotator::ZeroRotator,
				ActorSpawnParams
				);
		}

	}
#pragma endregion
}

void UAbilityComponent::CreatePortal()
{
	if (CreatePortalTimer > 0.f) return;
	CreatePortalTimer = CreatePortalTimerMax;


	if (PortalClass)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParam = {};
			SpawnParam.OverrideLevel = Character->GetLevel();
			SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParam.bDeferConstruction = true;

			Portal = World->SpawnActor<APortal>(
				PortalClass,
				Character->GetActorLocation(),
				Character->GetActorForwardVector().Rotation(),
				SpawnParam
				);

			Character->GetWorldTimerManager().SetTimer(
				PortalTimer,
				this,
				&UAbilityComponent::AddPortalSplinePoint,
				0.2f,
				true
			);
			//Portal->FinishSpawning(pMediaActor->GetTransform());
		}

		if (Character->GetPortalMaterial())
			Character->GetMesh()->SetMaterial(0, Character->GetPortalMaterial());
	}
	
}

void UAbilityComponent::AddPortalSplinePoint()
{
	Portal->AddSpline(Character->GetActorLocation());

	if (CreatePortalTimer <= 0.f)
	{
		if(Character->GetBaseMaterial())
			Character->GetMesh()->SetMaterial(0, Character->GetBaseMaterial());
		Character->GetWorldTimerManager().ClearTimer(PortalTimer);
		Portal->FinishSpawning(Portal->GetTransform());
		Portal = nullptr;
	}
}


void UAbilityComponent::StoreScanEnemy(UScanEnemyWidget* ScanEnemy, FVector Location)
{
	WidgetScanEnemys.Add(ScanEnemy, Location);

	FTimerHandle ScanEnemyTimer;
	FTimerDelegate ScanEnemyDelegate;
	ScanEnemyDelegate.BindUFunction(this, FName("DestroyScanEnemy"), ScanEnemy);
	Character->GetWorldTimerManager().SetTimer(
		ScanEnemyTimer,
		ScanEnemyDelegate,
		ScanEnemyDestroyTime,
		false
	);
}

void UAbilityComponent::DestroyScanEnemy(UScanEnemyWidget* ScanEnemy)
{
	WidgetScanEnemys.Remove(ScanEnemy);
	ScanEnemy->RemoveFromParent();
}

void UAbilityComponent::UpdateScanEnemys()
{
	for (auto& ScanEnemy : WidgetScanEnemys)
	{
		UScanEnemyWidget* ScanEnemyWidget = ScanEnemy.Key;

		FVector Location = ScanEnemy.Value;
		FVector2D ScreenPosition;
		UGameplayStatics::ProjectWorldToScreen(
			GetWorld()->GetFirstPlayerController(),
			Location,
			ScreenPosition
		);
		FVector2D WidgetSize = ScanEnemyWidget->GetWidgetSize();
		ScanEnemyWidget->SetPositionInViewport(ScreenPosition);
	}
}
