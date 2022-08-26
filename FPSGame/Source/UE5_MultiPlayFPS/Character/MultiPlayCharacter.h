// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "UE5_MultiPlayFPS/Types/TurningInPlace.h"
#include "UE5_MultiPlayFPS/Interface/InteractWithCrosshairsInterface.h"
#include "UE5_MultiPlayFPS/Interface/ZiplineInterface.h"
#include "UE5_MultiPlayFPS/Types/CombatState.h"
#include "UE5_MultiPlayFPS/Weapon/Weapon.h"
#include "MultiPlayCharacter.generated.h"


class USplineComponent;
UCLASS()
class UE5_MULTIPLAYFPS_API AMultiPlayCharacter : 
	public ACharacter, 
	public IInteractWithCrosshairsInterface ,
	public IZiplineInterface
	
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMultiPlayCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//컴포넌트 초기화 이후
	virtual void PostInitializeComponents() override;
	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	void PlayElimMontage();
	void PlayThrowGrenadeMontage(FName Selection=FName(TEXT("None")));
	//시뮬레이션된 프록시일 경우 사용
	//액터의 움직임이 변경되거나 복제될때 이곳에서 알림이 발생
	virtual void OnRep_ReplicatedMovement() override;
	void Elim();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Elim();
	UPROPERTY(Replicated)
	bool bDisableGameplay;

	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool bShowScape);

	void UpdateHUDHealth();
	void UpdateHUDShield();
	void UpdateInventoryAndAmmo();
	void Stun();

	void CreateDamageIndicatorHUD(ACharacter* HitCharacter, FVector& HitLocation);
	void SpawnDefaultWeapon();
	void PortalJump(USplineComponent* Spline, bool Reverse);
protected:
	virtual void Destroyed() override;
	virtual void BeginPlay() override;
	void MoveForward(float Value);
	void MoveRight(float Value);
	void LookUp(float Value);
	void TurnRight(float Value);
	void ZoomIn();
	//장착관련도 싹다 서버에서 관리
	void EquipBtnPressed();
	void CrouchBtnPressed();
	void CrouchBtnReleased();
	void SprintBtnPressed();
	void SprintBtnReleased();
	void ReloadBtnPressed();
	void AimBtnPressed();
	void AimBtnReleased();

	void SwapWeaponBtn1Pressed();
	void SwapWeaponBtn2Pressed();
	void SwapWeaponBtn3Pressed();
	void SwapWeaponBtn4Pressed();
	void SwapWeaponBtn5Pressed();

	void ScanBtnPressed();
	void AImOffset(float DT);

	void CalCulateAimOffset_Pitch();

	//회전시 끊김현상 시뮬레이션해서 완화
	//네트워크복제로 인한 업데이트는 실제로 훨씬 느리다.
	//그래서 시뮬레이션을 적용하여 자연스럽게 변경
	void SimProxiesTurn();
	virtual void Jump() override;

	void FireBtnPressed();
	void FireBtnReleased();
	void GrenadeBtnPressed();
	void GrenadeBtnReleased();
	void PlayHitReactMontage();
	UFUNCTION()
	void ReceiveDamage(AActor* DamageActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);
	//
	void PollInit();
	void RotateInPlace(float DeltaTime);

	void CreatePortalBtnPressed();
	void SmokeGrenadeBtnPressed();
	void ShopOnOffBtnPressed();
	UFUNCTION()
	void PortalMoveComponentTo();
	UFUNCTION()
	void PortalMoveFinish();


private:
	UPROPERTY(VisibleAnywhere,  meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* m_Cam;

	UPROPERTY(VisibleAnywhere,  meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* m_Arm;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	UPROPERTY(EditAnywhere,  meta = (AllowPrivateAccess = "true"))
	class UAIPerceptionStimuliSourceComponent* AIStimuliSourceComponent;
	//UPROPERTY(ReplicatedUsing= OnRep_OverlappingItem)
	class IPickupItemInterface* OverlappingItem;

	//파라미터는 값이 변경되지 건 마지막 값임
	//UFUNCTION()
	//void OnRep_OverlappingItem(IPickupItemInterface* LastItem);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* Combat1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UBuffComponent* Buff;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UParkourComponent* Parkour1;

	UPROPERTY(VisibleAnywhere,  meta = (AllowPrivateAccess = "true"))
	class UAbilityComponent* Ability;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* HandSceneComponent;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	class USplineComponent* SplineThrowComponent;


	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();
	
	float AimOffset_Yaw;
	float InterpAimOffset_Yaw;
	float AimOffset_Pitch;
	FRotator StartAimRot;
	
	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DT);
	
	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* ElimMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* ThrowMontage;

	void HideCameraToCharacter();
	UPROPERTY(EditAnywhere, Category = Camera)
	float CameraThreshold;

	bool bRotateRootBone;
	float TurnThreshold;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;
	float CalculateSpeed();

	/*
	Player Health
	*/
	UPROPERTY(EditAnywhere, Category="Player Stats")
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing= OnRep_Health, VisibleAnywhere, Category="Player Stats")
	float Health = 100.f;

	UFUNCTION()
	void OnRep_Health(float LastHealth);

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxShield = 100.f;
	UPROPERTY(ReplicatedUsing = OnRep_Shield, EditAnywhere, Category = "Player Stats")
	float Shield = 100.f;
	UFUNCTION()
	void OnRep_Shield(float LastShield);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class AMultiPlayPlayerController* MultiPlayPlayerController;

	bool bElimed;
	FTimerHandle ElimTimer;

	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.f;
	void ElimTimerFinished();
	
	/*
	/*
	Dissolve Effect TimeLine
	*/
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeLine;

	FOnTimelineFloat DissolveTrack;

	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurve;

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);
	void StartDissolve();
	
	UPROPERTY(VisibleAnywhere, Category=Elim)
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;

	UPROPERTY(EditAnywhere, Category=Elim)
	UMaterialInstance* DissolveMaterialInstance;

	UPROPERTY(VisibleAnywhere, Category = Elim)
		UMaterialInstanceDynamic* DynamicPaperBurnMaterialFaceInstance;

	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* DissolveMaterialFaceInstance;
	
	UPROPERTY()
	class AMultiPlayPlayerState* MultiPlayPlayerState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* AttachedGrenade;

	UPROPERTY(EditAnywhere, Category = Combat)
	float StunChance;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> DefaultWeaponClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UDamageIndicatorWidget> DamageIndicatorWidget;

	UPROPERTY(EditAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class UCameraShakeBase> CameraBoomShake;
	/*
	PortalJump
	*/
	float PortalDistance;
	class USplineComponent* PortalSpline;
	bool bPortalReverse;

	UPROPERTY(EditAnywhere, Category = "Portal")
	class UMaterial* PortalMaterial;
	class UMaterial* BaseMaterial;

	bool bShop;
public:
	void SetOverlappingItem(IPickupItemInterface* Item);
	USceneComponent* GetHandSceneComponent() { return HandSceneComponent; }
	bool IsWeaponEquipped();
	bool IsAiming();
	float GetAimOffset_Yaw() const { return AimOffset_Yaw; }
	float GetAimOffset_Pitch() const { return AimOffset_Pitch; }
	class AWeapon* GetEquippedWeapon();
	ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FVector GetHitTarget() const;
	UCameraComponent* GetCamera()const { return m_Cam; }
	bool ShouldRotateRootBone() const { return bRotateRootBone; }
	bool IsElimmed()const { return bElimed; }
	float GetHealth() const { return Health; }
	void SetHealth(float Amount) { Health = Amount; }
	float GetMaxHealth() const { return MaxHealth; }

	float GetShield() const { return Shield; }
	void SetShield(float Amount) { Shield = Amount; }
	float GetMaxShield() const { return MaxShield; }
	ECombatState GetCombatState() const;
	UCombatComponent* GetCombat() const { return Combat1; }
	bool GetDisableGameplay()const { return bDisableGameplay; }

	class UAnimMontage* GetReloadMontage() const { return ReloadMontage; }
	class UStaticMeshComponent* GetAttachedGrenade() const { return AttachedGrenade; }
	class UBuffComponent* GetBuff() const { return Buff; }
	class UParkourComponent* GetParkour() const { return Parkour1; }
	class UAbilityComponent* GetAbility() const { return Ability; }
	float GetStunChance()const { return StunChance; }

	void StartCameraShakeBoom(float Scale);

	virtual void SetZipline(bool Enable, const FVector& Start, const FVector& End);

	class USplineComponent* GetSplineThrowComponent(){return SplineThrowComponent;	}
	UMaterial* GetBaseMaterial() { return BaseMaterial; }
	UMaterial* GetPortalMaterial() { return PortalMaterial; }

	class AMultiPlayPlayerController* GetMultiPlayPlayerController() { return MultiPlayPlayerController; }
};

