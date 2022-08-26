// Fill out your copyright notice in the Description page of Project Settings.

#define CUSTOM_DEPTH_ENEMYBLUE 150
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UE5_MultiPlayFPS/Interface/BulletHitInterface.h"
#include "Enemy.generated.h"
UCLASS()
class UE5_MULTIPLAYFPS_API AEnemy : public ACharacter, public IBulletHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();

protected:
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
		class USphereComponent* CombatRangeSphere;

	UPROPERTY(VisibleAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* LeftWeaponCollision;

	UPROPERTY(VisibleAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* RightWeaponCollision;

	UPROPERTY(EditAnywhere, Category=Combat, meta=(AllowPrivateAccess= "true"))
	class UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
		class USoundCue* ImpactSound;
//protected:

	UPROPERTY(EditAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
		FString HeadBone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* HPBarWidget;

	UPROPERTY(EditAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float HealthBarDisplayTime;
	FTimerHandle HealthBarTimer;

	UPROPERTY(EditAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HitMontage;

	FTimerHandle HitReactTimer;

	float HitReactTime;
	bool bCanHitReact;

	UPROPERTY(VisibleAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TMap<UUserWidget*, FVector> HitNumbers;

	UPROPERTY(EditAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float HitNumberDestroyTime;

	UPROPERTY(EditAnywhere, Category = "AI", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, Category = "AI", meta = (AllowPrivateAccess = "true", MakeEditWidget="true"))
	FVector PatrolPoint;

	UPROPERTY(EditAnywhere, Category = "AI", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector PatrolPoint2;


	UPROPERTY(EditAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AgroSphere;

	FTimerHandle StunTimer;
	UPROPERTY(VisibleAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bStunned;
	
	UPROPERTY(EditAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float StunChance;

	UPROPERTY(EditAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float StunTime;
	UPROPERTY(VisibleAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bInAttackRange;



	UPROPERTY(VisibleAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bCanAttack;
	FTimerHandle AttackWaitTimer;
	UPROPERTY(EditAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float AttackWaitTime;


	FTimerHandle FireTimer;
	float FireTime;

	UPROPERTY(VisibleAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
		bool bDied;

	UPROPERTY(EditAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* DeathMontage;

	FTimerHandle DeathTimer;
	float DeathTime;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bFireCharacter;

	float ScanDepthIndex;
	FTimerHandle ScanDepthTimer;
	float ScanDepthTime;


	UPROPERTY(VisibleDefaultsOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
	class UAIPerceptionComponent* AIPerComp;

	UPROPERTY(VisibleDefaultsOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
	class UAISenseConfig_Sight* SightConfig;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	bool bDieRegdoll;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	int EnemyKillScore;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UEnemyHitNumberWidget> HitNumberWidgetClass;

	UPROPERTY(EditAnywhere)
	float FireRange;


protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float Health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float MaxHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Info")
		class UObjectPoolComponent* ObjectPoolBullet;
	UPROPERTY(EditAnywhere, Category = Combat)
		float BaseDamage;
	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* AttackMontage;

	class AEnemyAIController* EnemyAIController;
protected:
	virtual void Die();
	void PlayHitMontage(FName Section, float PlayRate = 1.f);
	void PlayDieMontage();
	void ResetHitReactTimer();

	void CreateHitNumberWidget(const int32 Damage, const FVector HitLocation, const bool bHeadShot);

	UFUNCTION(BlueprintCallable)
	void StoreHitNumber(UUserWidget* HitNumber, FVector Location);

	UFUNCTION()
	void DestroyHitNumber(UUserWidget* HitNumber);

	void UpdateHitNumbers();

	UFUNCTION()
	void AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	UFUNCTION()
	void CombatRangeSphereStartOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void AttackDamage(AActor* Victim);
	UFUNCTION()
	void CombatRangeSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	UFUNCTION()
	void OnLeftWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintCallable)
	void ActivateLeftWeapon();
	UFUNCTION(BlueprintCallable)
	void DeactivateLeftWeapon();
	UFUNCTION(BlueprintCallable)
	void ActivateRightWeapon();
	UFUNCTION(BlueprintCallable)
	void DeactivateRightWeapon();


	void StunCharacter(class AMultiPlayCharacter* Victim);

	void ResetCanAttack();

	UFUNCTION()
	void DestroyEnemy();
	void RegdollEvent();
	void EnableCustomDepth(bool bEnable);

	void FinishScanTimer();

	void Firing();

public:
	UFUNCTION(BlueprintCallable)
		virtual void FinishDeath();
	void FinishStun();

	UFUNCTION(BlueprintCallable)
	void SetStunned(bool Stunned);
	void ShowHealthBar();
	void HideHealthBar();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BulletHit_Implementation(FHitResult HitResult, AActor* HitActor) override;
	UFUNCTION()
		virtual void ReceiveDamage(AActor* DamageActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);

	FString GetHeadBone() { return HeadBone; }

	UBehaviorTree* GetBehaviorTree() { return BehaviorTree; }

	UFUNCTION()
		void PlayAttackMontage(FName Section, float PlayRate = 1.f);
	UFUNCTION()
	FName GetAttackSectionName();

	UFUNCTION(BlueprintCallable)
		void Fire();

	void Scanning();
	bool IsFireCharacter() { return bFireCharacter; }

	float GetHealth() { return Health; }
	
};
