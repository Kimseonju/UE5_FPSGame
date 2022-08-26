#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UE5_MultiPlayFPS/Widget/HUD/MultiPlayHUD.h"
#include "UE5_MultiPlayFPS/Weapon/WeaponTypes.h"
#include "UE5_MultiPlayFPS/Types/CombatState.h"
#include "CombatComponent.generated.h"

UENUM()
enum class EThrowType :uint8
{
	ETT_Grenade,
	ETT_SmokeGrenade
};

class AWeapon;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE5_MULTIPLAYFPS_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()
	friend class AMultiPlayCharacter;
public:	
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
		void EquipWeapon(AWeapon* WeaponToEquip);
	UFUNCTION(BlueprintCallable)
		void LoadEquipWeapon(AWeapon* WeaponToEquip);
	void SwapWeapon(int32 Index);
	void Reload();
	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	UFUNCTION(BlueprintCallable)
		void ShellReloading();
	void JumpToShotGunEnd();
	UFUNCTION(BlueprintCallable)
	void PickupAmmo(EWeaponType WeaponType, int Ammo);

	UFUNCTION(BlueprintCallable)
	void EndStun();
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY()
	class AMultiPlayCharacter* Character;
	UPROPERTY()
	class AMultiPlayPlayerController* Controller;
	UPROPERTY()
	class AMultiPlayHUD* HUD;

	UPROPERTY(ReplicatedUsing= OnRep_EquippedWeapon, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	AWeapon* EquipedWeapon;

	UFUNCTION()
	void OnRep_EquippedWeapon();
	UPROPERTY(Replicated
	bool bAiming;

	UPROPERTY(EditAnywhere)
	float RunSpeed;
	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;
	
	bool bFireBtnPressed;

	bool bTickUpdate;
	//HUD
	float CrosshairRange;
	float CrosshairInAirRange;
	float CrosshairAimRange;
	float CrosshairShootingRange;
	FVector HitTarget;
	FHUDPackage HUDPackage;


	float DefaultFOV;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomFOV;

	float CurrentFOV;
	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomInterpSpeed;
	//줌인 줌아웃
	void InterpFOV(float DeltaTime);

	FTimerHandle FireTimer;
	bool bCanFire;
	float FireDelay;
	float FireDelayMax;

	void StartFireTimer();
	void FireTimerFinished();
	bool CanFire();

	UPROPERTY(ReplicatedUsing=OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	UFUNCTION()
	void OnRep_CarriedAmmo();
	//해시를 사용하기때문에 복제를 할 수 없음 (Replicated)
	TMap<EWeaponType, int32> CarriedAmmoMap;

	UPROPERTY(EditAnywhere)
	int32 MaxCarriedAmmo;

	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo;

	UPROPERTY(EditAnywhere)
	int32 StartingRocketAmmo;

	UPROPERTY(EditAnywhere)
		int32 StartingPistolAmmo;

	UPROPERTY(EditAnywhere)
	int32 StartingSMGAmmo;

	UPROPERTY(EditAnywhere)
		int32 StartingShotGunAmmo;
	UPROPERTY(EditAnywhere)
		int32 StartingSniperRifleAmmo;
	UPROPERTY(EditAnywhere)
		int32 StartingGrenadeLauncherAmmo;
	void InitializeCarriedAmmo();
	
	UPROPERTY(ReplicatedUsing=OnRep_CombatState)
	ECombatState CombatState;

	UFUNCTION()
	void OnRep_CombatState();

	void UpdateAmmoValues();
	void UpdateShellAmmoValues();

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Grenades)
	int Grenades;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	bool Throwing;
	EThrowType ThrowType;
	TArray<class USplineMeshComponent*> SplineMeshThrows;

	UPROPERTY(EditAnywhere)
	class UStaticMesh* SplineThrowDrawStaticMesh;
	UPROPERTY(EditAnywhere)
	class UMaterial* SplineThrowDrawMaterial;
	FTimerHandle ThrowDrawTimer;
	UPROPERTY(EditAnywhere)
	float ThrowDrawTime;
	UFUNCTION()
	void OnRep_Grenades();
	UPROPERTY(EditAnywhere)
		int MaxGrenades;
	void UpdateHUDGrenades();

	//Inventory

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Inventory, meta = (AllowPrivateAccess = "true"))
	TArray<AWeapon*> Inventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
		int32 SlotIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	int32 MaxSlotIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	int32 INVENTORY_CAPACITY= 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UCameraShakeBase> CameraFireShake;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UCameraShakeBase> CameraRecoilShake;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FTransform ClipTransform;
	bool bGrabMovingClip;
public:

	void AttachActorToLeftHand(AActor* ActorToAttach);
	void AttachActorToRightHand(AActor* ActorToAttach);
	void AttachActorToBack(AActor* ActorToAttach);
protected:
	FORCEINLINE void DropEquippedWeapon();
	void UpdateCarriedAmmo();
	void PlayEquipWeaponSound();
	void ReloadZeroAmmoWeapon();
	void ShowAttachedGrenade(bool bShowGrenade);

	void SetAiming(bool bIsAiming);
	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	void Fire();
	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(Server, Reliable)
		void ServerReload();
	void HandleReload();

	int32 AmountToReload();
	void ThrowDraw();
	void ThrowDrawFinish();
	void ThrowGrenade(EThrowType Type);
	UFUNCTION(BlueprintCallable)
		void FinishThrowGrenade();
	UFUNCTION(BlueprintCallable)
		void GrenadeLaunch();
	UFUNCTION(Server, Reliable)
	void ServerThrowGrenade();

	//FVector_NetQuantize// 60비트 벡터는 96비트
	UFUNCTION(Server, Reliable)
	void ServerLaunchGrenade(const FVector_NetQuantize& Target, EThrowType Type);
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> GrenadeClass;
	UPROPERTY(EditAnywhere)
		TSubclassOf<class AProjectile> SmokeGrenadeClass;

	void TraceCrosshairs(FHitResult& TraceHitResult);
	void SetHUDCrosshairs(float DT);


	UFUNCTION(BlueprintCallable)
	void GrabClip();

	UFUNCTION(BlueprintCallable)
	void GrabClipFinish();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerAddGrenade(int Grenade);
	UFUNCTION(BlueprintCallable)
	void AddThrowGrenades();
public:
	UFUNCTION(BlueprintCallable)
	void ReadyThrow();
	void FireBtnPressed(bool bPressed);
	int GetGrands() const { return Grenades; }
	bool IsGrabMovingClip() const { return bGrabMovingClip; }

	TArray<AWeapon*> GetInventory() { return Inventory;}
	int32 GetSlotIndex() { return SlotIndex; }
	void SetSlotIndex(int Index) { SlotIndex = Index; }
	int GetCarriedAmmo(EWeaponType Type)
	{
		return CarriedAmmoMap[Type];
	}

	UFUNCTION(BlueprintCallable)
	void SetThrowing(bool bThrowing) { Throwing = bThrowing; }
	bool GetThrowing()const { return Throwing; }
};
