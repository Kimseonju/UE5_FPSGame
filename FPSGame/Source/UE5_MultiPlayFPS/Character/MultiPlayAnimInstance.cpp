// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiPlayAnimInstance.h"
#include "MultiPlayCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "UE5_MultiPlayFPS/Weapon/Weapon.h"
#include "Net/UnrealNetwork.h"
#include "UE5_MultiPlayFPS/Types/CombatState.h"
#include "UE5_MultiPlayFPS/MultiPlayComponents/ParkourComponent.h"
#include "UE5_MultiPlayFPS/MultiPlayComponents/CombatComponent.h"
void UMultiPlayAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	MultiPlayCharacter = Cast<AMultiPlayCharacter>(TryGetPawnOwner());
}

void UMultiPlayAnimInstance::NativeBeginPlay()
{

}

void UMultiPlayAnimInstance::NativeUpdateAnimation(float _fDT)
{
	Super::NativeUpdateAnimation(_fDT);
	if (MultiPlayCharacter == nullptr)
	{
		MultiPlayCharacter= Cast<AMultiPlayCharacter>(TryGetPawnOwner());
	}
	if (MultiPlayCharacter == nullptr)return;
	if (ParkourComponent == nullptr)
	{
		ParkourComponent = MultiPlayCharacter->GetParkour();
	}
	if(ParkourComponent==nullptr) return;
	FVector Velocity = MultiPlayCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = MultiPlayCharacter->GetCharacterMovement()->IsFalling() || MultiPlayCharacter->GetCharacterMovement()->IsFlying();
	bIsAccelerating = MultiPlayCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	bWeaponEquipped = MultiPlayCharacter->IsWeaponEquipped();
	EquippedWeapon = MultiPlayCharacter->GetEquippedWeapon();
	bIsCrouch = MultiPlayCharacter->GetCharacterMovement()->IsCrouching();
	bAiming = MultiPlayCharacter->IsAiming();
	bRotateRootBone = MultiPlayCharacter->ShouldRotateRootBone();
	bElimed = MultiPlayCharacter->IsElimmed();

	FRotator AimRotation = MultiPlayCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(MultiPlayCharacter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, _fDT, 6.f);
	YawOffset = DeltaRotation.Yaw;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = MultiPlayCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / _fDT;
	const float Interp = FMath::FInterpTo(Lean, Target, _fDT, 6.f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);
	AimOffset_Yaw = MultiPlayCharacter->GetAimOffset_Yaw();
	AimOffset_Pitch = MultiPlayCharacter->GetAimOffset_Pitch();
	TurningInPlace = MultiPlayCharacter->GetTurningInPlace();
	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && MultiPlayCharacter->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		MultiPlayCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator
			, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		if (MultiPlayCharacter->IsLocallyControlled())
		{
			bLocallyController = true;
			FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("hand_r"), ERelativeTransformSpace::RTS_World);
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - BlasterCharacter->GetHitTarget()));
			RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, DeltaTime, 30.f);
		}

		FTransform MuzzleTipTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform("MuzzleFlash", ERelativeTransformSpace::RTS_World);
		FVector MuzzleX(FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));
		//DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), MuzzleTipTransform.GetLocation() + MuzzleX * 1000.f, FColor::Red);
		//DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), MultiPlayCharacter->GetHitTarget(), FColor::Orange);
	}

	bUseFABRIK = MultiPlayCharacter->GetCombatState() == ECombatState::ECS_Unoccupied;
	bUseAimOffset = MultiPlayCharacter->GetCombatState() == ECombatState::ECS_Unoccupied && !MultiPlayCharacter->GetDisableGameplay();
	bTransformRightHand = MultiPlayCharacter->GetCombatState() == ECombatState::ECS_Unoccupied && !MultiPlayCharacter->GetDisableGameplay();

	if (MultiPlayCharacter->GetCombat())
	{
		bGrabMovingClip = MultiPlayCharacter->GetCombat()->IsGrabMovingClip();
	}
}

void UMultiPlayAnimInstance::AnimNotify_GrenadeThrowReady()
{
	if (MultiPlayCharacter)
	{
		MultiPlayCharacter->GetCombat()->ReadyThrow();
	}
}
