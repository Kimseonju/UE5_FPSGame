// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponTypes.h"
#include "UE5_MultiPlayFPS/Interface/PickupItemInterface.h"
#include <Engine/DataTable.h>
#include "Weapon.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FWeaponInfo
	: public FTableRowBase // 데이터 테이블과 연동되는 구조체
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Info)
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Image;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Info)
	float MaxAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Info)
	int Price;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Info)
	EWeaponType Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Info)
	TSubclassOf<AWeapon> WeaponClass;
};

UENUM(BlueprintType)
enum class EWeaponState :uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_PickedUp UMETA(DisplayName = "PickedUp"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),
	EWS_Default UMETA(DisplayName = "Default"),
};
UCLASS()
class UE5_MULTIPLAYFPS_API AWeapon : public AActor, public IPickupItemInterface
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Owner() override;
	void SetHUDAmmo();
	UFUNCTION(BlueprintCallable)
	virtual void Fire(const FVector& HitTarget);
	void Dropped();
	void AddAmmo(int32 AmmoToAdd);
	void EnableCustomDepth(bool bEnable);
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);
private:
	UPROPERTY(VisibleAnywhere, Category="Weapon Info")
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Info")
	class USphereComponent* AreaSphere;


	UPROPERTY(ReplicatedUsing=OnRep_WeaponState,VisibleAnywhere, Category = "Weapon Info")
	EWeaponState WeaponState;
	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(EditAnywhere, Category="Weapon Info")
	class UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACasing> CasingClass;

	UPROPERTY(EditAnywhere, ReplicatedUsing=OnRep_Ammo)
	int32 Ammo;

	UPROPERTY(EditAnywhere, Category = "Decal")
	class UMaterial* FireDecalMaterial;
	UPROPERTY(EditAnywhere, Category = "Decal")
	FVector DecalSize;
	UPROPERTY(EditAnywhere, Category = "Decal")
	float DecalLifeTime;
	void SpendRound();
	UFUNCTION()
	void OnRep_Ammo();

	UPROPERTY(EditAnywhere)
	int32 Magcapacity;

	UPROPERTY()
	class AMultiPlayCharacter* MultiPlayCharacter;
	UPROPERTY()
	class AMultiPlayPlayerController* MultiPlayPlayerController;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Info")
		FItemInfo ItemInfo;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Info")
	FName ClipBoneName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Info")
		class UWidgetComponent* PickupWidget;
public:

	//HUD
	UPROPERTY(EditAnywhere, Category = Crosshair)
		class UTexture2D* CrosshairCenter;

	UPROPERTY(EditAnywhere, Category = Crosshair)
		class UTexture2D* CrosshairLeft;

	UPROPERTY(EditAnywhere, Category = Crosshair)
		class UTexture2D* CrosshairRight;

	UPROPERTY(EditAnywhere, Category = Crosshair)
		class UTexture2D* CrosshairTop;

	UPROPERTY(EditAnywhere, Category = Crosshair)
		class UTexture2D* CrosshairBottom;

	UPROPERTY(EditAnywhere)
	float ZoomFOV;
	
	UPROPERTY(EditAnywhere)
	float ZoomInterpSpeed;

	UPROPERTY(EditAnywhere, Category = Combat)
	float FireDelay;

	UPROPERTY(EditAnywhere, Category = Combat)
	bool bAutomatic;
	
	UPROPERTY(EditAnywhere)
	class USoundCue* EquipSound;

public:	
	void SetWeaponState(EWeaponState State);
	EWeaponState GetWeaponState() { return WeaponState; }
	virtual void ShowPickupWidget(bool bShowWidget) override;
	class USphereComponent* GetAreaSphere()const { return AreaSphere; }
	USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	float GetZoomFOV() const { return ZoomFOV; }
	float GetZoomInterpSpeed()const { return ZoomInterpSpeed; }

	bool IsAmmo();
	EWeaponType GetWeaponType() const {return WeaponType;}
	FItemInfo GetItemInfo() const { return ItemInfo; }
	int32 GetAmmo() const { return Ammo; }
	int32 GetMagCapacity() const { return Magcapacity; }
	bool IsFull();
	FName GetClipBoneName() const { return ClipBoneName; }
};
