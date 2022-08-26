// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class UE5_MULTIPLAYFPS_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float _fDT) override;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float Speed;
	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		bool bIsInAir;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		bool bIsAccelerating;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class AEnemy* EnemyCharacter;
public:
	UFUNCTION()
		void AnimNotify_Fire();
	UFUNCTION()
		void AnimNotify_EndDeath();
	UFUNCTION()
		void AnimNotify_StunFinish();


	/*
	Boss Notify
	*/
	UFUNCTION()
	void AnimNotify_EnemyRecall();

	UFUNCTION()
	void AnimNotify_MeleeAttack();

	UFUNCTION()
	void AnimNotify_FieldSkill();

	UFUNCTION()
	void AnimNotify_FieldSkillFinish();

	UFUNCTION()
	void AnimNotify_Explode();


	UFUNCTION()
	void AnimNotify_Dash();

	UFUNCTION()
	void AnimNotify_DashFinish();
};
