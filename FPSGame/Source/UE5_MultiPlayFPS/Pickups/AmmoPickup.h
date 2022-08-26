// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "UE5_MultiPlayFPS/Weapon/WeaponTypes.h"
#include <Engine/DataTable.h>
#include "AmmoPickup.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FPickAmmoInfo
	: public FTableRowBase // 데이터 테이블과 연동되는 구조체
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Info, meta = (AllowPrivateAccess = "true"))
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UTexture2D* Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Info, meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Info, meta = (AllowPrivateAccess = "true"))
	float Ammo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Info, meta = (AllowPrivateAccess = "true"))
	int Price;
};

/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API AAmmoPickup : public APickup
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
private:
	UPROPERTY(EditAnywhere)
	int Ammo;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;

	virtual void EquippedEvent(class AMultiPlayCharacter* Player) override;
};
