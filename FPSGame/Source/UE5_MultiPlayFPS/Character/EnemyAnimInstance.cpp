// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"
#include "UE5_MultiPlayFPS/Character/Enemy.h"
#include "UE5_MultiPlayFPS/Character/BossEnemy.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
void UEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

}

void UEnemyAnimInstance::NativeBeginPlay()
{

}

void UEnemyAnimInstance::NativeUpdateAnimation(float _fDT)
{
	Super::NativeUpdateAnimation(_fDT);
	if (EnemyCharacter == nullptr)
	{
		EnemyCharacter = Cast<AEnemy>(TryGetPawnOwner());
	}
	if (EnemyCharacter == nullptr)return;

	if (EnemyCharacter)
	{
		FVector Velocity = EnemyCharacter->GetVelocity();
		Velocity.Z = 0.f;
		Speed = Velocity.Size();
		bIsAccelerating = Speed > 0.f ? true : false;
		bIsInAir = EnemyCharacter->GetCharacterMovement()->IsFalling();
	}

}

void UEnemyAnimInstance::AnimNotify_Fire()
{
	if (EnemyCharacter)
	{
		EnemyCharacter->Fire();
	}
}

void UEnemyAnimInstance::AnimNotify_EndDeath()
{
	if (EnemyCharacter)
	{
		EnemyCharacter->FinishDeath();
	}
}

void UEnemyAnimInstance::AnimNotify_StunFinish()
{
	if (EnemyCharacter)
	{
		EnemyCharacter->SetStunned(false);
	}
}

void UEnemyAnimInstance::AnimNotify_EnemyRecall()
{
	ABossEnemy* BossEnemy=Cast<ABossEnemy>(EnemyCharacter);
	if (BossEnemy)
	{
		BossEnemy->BossEnemyRecall();
	}
}

void UEnemyAnimInstance::AnimNotify_MeleeAttack()
{
	ABossEnemy* BossEnemy = Cast<ABossEnemy>(EnemyCharacter);
	if (BossEnemy)
	{
		BossEnemy->PlayAttackMontage("AttackCombo1");
	}
}

void UEnemyAnimInstance::AnimNotify_FieldSkill()
{
	ABossEnemy* BossEnemy = Cast<ABossEnemy>(EnemyCharacter);
	if (BossEnemy)
	{
		BossEnemy->BossFieldSkill();
	}
}

void UEnemyAnimInstance::AnimNotify_FieldSkillFinish()
{
	ABossEnemy* BossEnemy = Cast<ABossEnemy>(EnemyCharacter);
	if (BossEnemy)
	{
		BossEnemy->EndFieldSkill();
	}
}

void UEnemyAnimInstance::AnimNotify_Explode()
{
	ABossEnemy* BossEnemy = Cast<ABossEnemy>(EnemyCharacter);
	if (BossEnemy)
	{
		BossEnemy->BossExplode();
	}
}

void UEnemyAnimInstance::AnimNotify_Dash()
{
	ABossEnemy* BossEnemy = Cast<ABossEnemy>(EnemyCharacter);
	if (BossEnemy)
	{
		BossEnemy->Dash();
	}
}

void UEnemyAnimInstance::AnimNotify_DashFinish()
{
	ABossEnemy* BossEnemy = Cast<ABossEnemy>(EnemyCharacter);
	if (BossEnemy)
	{
		BossEnemy->DashFinish();
	}
}
