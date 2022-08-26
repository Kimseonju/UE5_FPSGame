// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "BossEnemy.generated.h"

UENUM()
enum BossPattern
{
	EP_FieldSkill,
	EP_EnemyRecall,
	EP_PullTarget,
	EP_Explode,
	EP_END
};
/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API ABossEnemy : public AEnemy
{
	GENERATED_BODY()
public:
	ABossEnemy();
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;



private:

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* SceneComp;

	UPROPERTY(VisibleAnywhere)
		class USceneComponent* Target1;
	UPROPERTY(VisibleAnywhere)
		class USceneComponent* Target2;
	UPROPERTY(VisibleAnywhere)
		class USceneComponent* FollowTarget;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
		class UNiagaraComponent* DashComponent;

	UPROPERTY(EditAnywhere, Category = "Skill_Dash", meta = (AllowPrivateAccess = "true"))
	class UNiagaraSystem* DashEffect;

	UPROPERTY(EditAnywhere, Category = "Skill", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ASkillDecal> SkillDecalClass;

	UPROPERTY(EditAnywhere,  Category = "Skill_Field", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* FieldImpactParticle;

	UPROPERTY(EditAnywhere,  Category = "Skill_Field", meta = (AllowPrivateAccess = "true"))
		class UParticleSystem* EnemyRecallParticle;
	UPROPERTY(EditAnywhere,  Category = "Skill_Field", meta = (AllowPrivateAccess = "true"))
	class USoundCue* FieldImpactSound;

	UPROPERTY(EditAnywhere,  Category = "Skill_Field", meta=(AllowPrivateAccess= "true"))
	float FieldSkillActorLifeTime;
	UPROPERTY(EditAnywhere,  Category = "Skill_Field", meta = (AllowPrivateAccess = "true"))
	float FieldSkillRecallTimeMax;
	UPROPERTY(EditAnywhere,  Category = "Skill_Field", meta = (AllowPrivateAccess = "true"))
	FVector FieldSkillRange;

	FTimerHandle FieldSkillHandle;

	FTimerHandle PullTargetSkillHandle;
	UPROPERTY(EditAnywhere,  Category = "Skill_Recall", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class AEnemy>> ArrEnemyClass;
	UPROPERTY(EditAnywhere,  Category = "Skill_Recall", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector RecallPoint1;
	UPROPERTY(EditAnywhere,  Category = "Skill_Recall", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector RecallPoint2;

	int MeleeAttack;
	int MeleeAttackMax;

	UPROPERTY(EditAnywhere,  Category = "Skill_Recall", meta = (AllowPrivateAccess = "true"))
	float DashRange;
	UPROPERTY(EditAnywhere,  Category = "Skill_Recall", meta = (AllowPrivateAccess = "true"))
	float MeleeAttackRange;
	UPROPERTY(EditAnywhere,  Category = "Skill_Dash", meta = (AllowPrivateAccess = "true"))
	float BaseSpeed;
	UPROPERTY(EditAnywhere,  Category = "Skill_Dash", meta = (AllowPrivateAccess = "true"))
	float DashSpeed;
	UPROPERTY(EditAnywhere, Category = "Skill_Explode", meta = (AllowPrivateAccess = "true"))
		float  ExplodeDamageInnerRadius;
	UPROPERTY(EditAnywhere, Category = "Skill_Explode", meta = (AllowPrivateAccess = "true"))
		float  ExplodeDamageOuterRadius;
	int PatternCount;

	bool IsPullTargetCol;
	UPROPERTY(EditAnywhere, Category = "Pattern", meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> BossMove;
	FVector SaveLocation;
	bool bSaveLocation;
protected:

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Weapon Info")
		class UObjectPoolComponent* ObjectPoolCylinder;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Info")
		class UObjectPoolComponent* ObjectPoolField;

protected:
	virtual void ReceiveDamage(AActor* DamageActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);

public:
	void StartFieldSkill();

	void FieldSkill();

	void EndFieldSkill();
	void EnemyRecall();


	void BossExplode();

	void BossAttack(BossPattern Pattern);
	void BossEnemyRecall();
	void BossFieldSkill();
	void BossPullTarget();
	void Dash();
	void DashFinish();
	void DashMove_Fly();
	UFUNCTION()
		void DashMove_FlyNext();
	void DashMove_FlyEnd();
	void FireEvent();
	void Firing();


	UFUNCTION()
		void LoopTarget1();
	UFUNCTION()
		void LoopTarget2();

	void LeftRightFire();
	UFUNCTION(BlueprintCallable)
	void CircleFire();

	UFUNCTION(BlueprintCallable)
	void CylinderFire();
	virtual void FinishDeath();
	void Attack1();
	float GetDashRange() const { return DashRange; }
	float GetMeleeAttackRange()const { return MeleeAttackRange; }
	int GetPatternCount()const {return PatternCount;}
};
