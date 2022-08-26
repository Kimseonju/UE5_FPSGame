#include "CombatComponent.h"
#include "UE5_MultiPlayFPS/Character/MultiPlayCharacter.h"
#include "UE5_MultiPlayFPS/Weapon/Weapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "UE5_MultiPlayFPS/PlayerController/MultiPlayPlayerController.h"
#include "UE5_MultiPlayFPS/Widget/HUD/MultiPlayHUD.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"
#include "UE5_MultiPlayFPS/Character/MultiPlayAnimInstance.h"
#include "UE5_MultiPlayFPS/Weapon/Projectile.h"
#include "UE5_MultiPlayFPS/Interface/BulletHitInterface.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	RunSpeed = 600.f;
	AimWalkSpeed = 450.f;
	ZoomFOV = 30.f;
	ZoomInterpSpeed = 20.f;
	bCanFire = true;
	StartingARAmmo = 30;
	CombatState = ECombatState::ECS_Unoccupied;
	StartingRocketAmmo = 30;
	StartingPistolAmmo = 30;
	StartingSMGAmmo = 90.f;
	StartingShotGunAmmo = 90.f;
	StartingSniperRifleAmmo = 0.f;
	StartingGrenadeLauncherAmmo = 0.f;
	Grenades = 5;
	MaxGrenades = 5;
	MaxCarriedAmmo = 500;
	MaxSlotIndex = 0;
	Inventory.Init(nullptr, 5);
	FireDelayMax = 1.f;
	FireDelay = 1.f;
	Throwing = false;
	ThrowDrawTime = 0.5f;
	bTickUpdate = false;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCombatComponent, EquipedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
	//다른 클라X 활성 클라이언트에게만 복제
	DOREPLIFETIME_CONDITION(UCombatComponent, CarriedAmmo, COND_OwnerOnly);
	DOREPLIFETIME(UCombatComponent, CombatState);
	DOREPLIFETIME(UCombatComponent, Grenades);
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
		if (Character->GetCamera())
		{
			DefaultFOV = Character->GetCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
		UpdateHUDGrenades();
		if (Character->HasAuthority())
		{
			InitializeCarriedAmmo();
		}
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, 
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//자율 프록시 또는 로컬로 제어되는 서버 캐릭터
	if (Character && Character->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceCrosshairs(HitResult);
		HitTarget = HitResult.ImpactPoint;
		SetHUDCrosshairs(DeltaTime);
		InterpFOV(DeltaTime);
		if (HitResult.bBlockingHit)
		{
			IPickupItemInterface* Item = Cast<IPickupItemInterface>(HitResult.GetActor());
			AActor* Actor= HitResult.GetActor();
			if (HitResult.GetActor() && Item && Item->bOverlap)
			{
				Character->SetOverlappingItem(Item);
				Item->ShowPickupWidget(true);
			}
			else
			{
				Character->SetOverlappingItem(nullptr);
			}
		}
		else
		{
			Character->SetOverlappingItem(nullptr);
		}
	}

#pragma region FireDelay
	FireDelay += DeltaTime;
	if (FireDelay >= FireDelayMax)
	{
		FireDelay = FireDelayMax;
	}
#pragma endregion
#pragma region PrevTickUpdate
	if (!bTickUpdate)
	{
		bTickUpdate = true;

		UpdateHUDGrenades();
	}
#pragma endregion
}

void UCombatComponent::SetHUDCrosshairs(float DT)
{
#pragma region nullptrCheck

	if (Character == nullptr || Character->Controller == nullptr) return;
	if (Controller == nullptr)
	{
		Controller = Cast<AMultiPlayPlayerController>(Character->Controller);
	}
#pragma endregion

	if (Controller)
	{
		if (HUD == nullptr)
			HUD = Cast<AMultiPlayHUD>(Controller->GetHUD());
		if (HUD)
		{
			if (EquipedWeapon && EquipedWeapon->GetWeaponType() != EWeaponType::EWT_SniperRifle)
			{
				HUDPackage.CrosshairCenter = EquipedWeapon->CrosshairCenter;
				HUDPackage.CrosshairLeft = EquipedWeapon->CrosshairLeft;
				HUDPackage.CrosshairRight = EquipedWeapon->CrosshairRight;
				HUDPackage.CrosshairTop = EquipedWeapon->CrosshairTop;
				HUDPackage.CrosshairBottom = EquipedWeapon->CrosshairBottom;
			}
			else
			{
			
				HUDPackage.CrosshairCenter = nullptr;
				HUDPackage.CrosshairLeft = nullptr;
				HUDPackage.CrosshairRight = nullptr;
				HUDPackage.CrosshairTop = nullptr;
				HUDPackage.CrosshairBottom = nullptr;
			}
			HUDPackage.CrosshairSpread;
			//범위를 0~1로 
			FVector2D WalkSpeedRange(0.f, RunSpeed);
			FVector2D Range(0.f, 1.f);
			FVector Velocity = Character->GetVelocity();
			Velocity.Z = 0.f;

			CrosshairRange =FMath::GetMappedRangeValueClamped(WalkSpeedRange, Range, Velocity.Size());
			if (Character->GetCharacterMovement()->IsFalling())
			{
				CrosshairInAirRange = FMath::FInterpTo(CrosshairInAirRange, 2.25f, DT, 2.25f);
			}
			else
			{
				CrosshairInAirRange = FMath::FInterpTo(CrosshairInAirRange, 0.f, DT, 20.f);
			}
			if (bAiming)
			{
				CrosshairAimRange=FMath::FInterpTo(CrosshairAimRange, 0.28f, DT, 30.f);
			}
			else
			{
				CrosshairAimRange = FMath::FInterpTo(CrosshairAimRange, 0.f, DT, 30.f);
			}
			CrosshairShootingRange = FMath::FInterpTo(CrosshairShootingRange, 0.f, DT, 40.f);
			HUDPackage.CrosshairSpread = 0.2f+CrosshairRange + CrosshairInAirRange - 
				CrosshairAimRange + CrosshairShootingRange;
			HUD->SetHUDPackage(HUDPackage);
		}
	}
}

void UCombatComponent::GrabClip()
{
	if (EquipedWeapon == nullptr) return;
	if (Character->GetHandSceneComponent()==nullptr) return;
	int32 ClipBoneIndex = EquipedWeapon->GetWeaponMesh()->GetBoneIndex(EquipedWeapon->GetClipBoneName());
	ClipTransform = EquipedWeapon->GetWeaponMesh()->GetBoneTransform(ClipBoneIndex);

	FAttachmentTransformRules AttackmentRules(EAttachmentRule::KeepRelative, true);
	Character->GetHandSceneComponent()->AttachToComponent(Character->GetMesh(), AttackmentRules, FName(TEXT("Hand_L")));
	Character->GetHandSceneComponent()->SetWorldTransform(ClipTransform);
	bGrabMovingClip = true;
}

void UCombatComponent::GrabClipFinish()
{
	bGrabMovingClip = false;
}


void UCombatComponent::InterpFOV(float DeltaTime)
{
	if (EquipedWeapon == nullptr) return;

	if (bAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquipedWeapon->GetZoomFOV(), DeltaTime, EquipedWeapon->GetZoomInterpSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, ZoomInterpSpeed);
	}
	if (Character && Character->GetCamera())
	{
		Character->GetCamera()->SetFieldOfView(CurrentFOV);
	}
}

void UCombatComponent::SetAiming(bool bIsAiming)
{
	if (Character == nullptr || EquipedWeapon == nullptr) return;
	if (!(CombatState == ECombatState::ECS_Unoccupied || CombatState == ECombatState::ECS_Reloading)) return;
	if (bAiming == bIsAiming) return;
	bAiming = bIsAiming;
	ServerSetAiming(bIsAiming);
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : RunSpeed;
	}

	if (Character->IsLocallyControlled() && EquipedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle)
	{
		Character->ShowSniperScopeWidget(bIsAiming);
	}
}

bool UCombatComponent::CanFire()
{
	if (EquipedWeapon == nullptr) return false;
	if (EquipedWeapon->IsAmmo() && bCanFire && CombatState == ECombatState::ECS_Reloading && EquipedWeapon->GetWeaponType() == EWeaponType::EWT_ShotGun) return true;
	return EquipedWeapon->IsAmmo() && bCanFire && CombatState == ECombatState::ECS_Unoccupied;
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	Controller = Controller == nullptr ? Cast<AMultiPlayPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}

	bool bJumpToShotGunEnd =
		CombatState == ECombatState::ECS_Reloading &&
		EquipedWeapon != nullptr &&
		EquipedWeapon->GetWeaponType() == EWeaponType::EWT_ShotGun &&
		CarriedAmmo == 0;
	if (bJumpToShotGunEnd)
	{
		JumpToShotGunEnd();
	}
}

void UCombatComponent::InitializeCarriedAmmo()
{
	CarriedAmmoMap.Emplace(EWeaponType::EWT_AssaultRifle, StartingARAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_RocketLauncher, StartingRocketAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Pistol, StartingPistolAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_SMG, StartingSMGAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_ShotGun, StartingShotGunAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_SniperRifle, StartingSniperRifleAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_GrenadeLauncher, StartingGrenadeLauncherAmmo);
	
}


void UCombatComponent::FireBtnPressed(bool bPressed)
{
	bFireBtnPressed = bPressed;
	if (bFireBtnPressed && EquipedWeapon)
	{
		Fire();
	}
}


void UCombatComponent::Fire()
{
	if (CanFire())
	{
		bCanFire = false;
		ServerFire(HitTarget);
		if (EquipedWeapon)
		{
			CrosshairShootingRange += 0.75f;
		}

		if (CameraFireShake)
		{
			Controller->ClientStartCameraShake(CameraFireShake, 1.f);
		}
		if (CameraRecoilShake)
		{
			Controller->ClientStartCameraShake(CameraRecoilShake, 1.f);
		}
		StartFireTimer();
	}
}

void UCombatComponent::StartFireTimer()
{
	if (EquipedWeapon == nullptr || Character == nullptr)return;
	Character->GetWorldTimerManager().SetTimer(
		FireTimer,
		this,
		&UCombatComponent::FireTimerFinished,
		EquipedWeapon->FireDelay
	);
	FireDelay = 0.f;
	FireDelayMax = EquipedWeapon->FireDelay;
}

void UCombatComponent::FireTimerFinished()
{
	if (EquipedWeapon == nullptr)return;
	bCanFire = true;
	if (bFireBtnPressed && EquipedWeapon->bAutomatic)
	{
		Fire();
	}
	ReloadZeroAmmoWeapon();
}

void UCombatComponent::Reload()
{
	if (CarriedAmmo > 0 && CombatState == ECombatState::ECS_Unoccupied && EquipedWeapon && !EquipedWeapon->IsFull())
	{
		ServerReload();
		SetAiming(false);
	}
}

void UCombatComponent::FinishReloading()
{
	if (Character == nullptr) return;
	if (CombatState == ECombatState::ECS_Stunned) return;
	if (Character->HasAuthority())
	{
		CombatState = ECombatState::ECS_Unoccupied;
		UpdateAmmoValues();
	}
	if (bFireBtnPressed)
	{
		Fire();
	}
}

void UCombatComponent::ShellReloading()
{
	if (Character->HasAuthority())
	{
		UpdateShellAmmoValues();
	}
}

void UCombatComponent::PickupAmmo(EWeaponType WeaponType, int Ammo)
{
	if (CarriedAmmoMap.Contains(WeaponType))
	{
		CarriedAmmoMap[WeaponType] =FMath::Clamp(CarriedAmmoMap[WeaponType]+ Ammo, 0, MaxCarriedAmmo);

		UpdateCarriedAmmo();
	}
	if (EquipedWeapon && !EquipedWeapon->IsAmmo() && EquipedWeapon->GetWeaponType() == WeaponType)
	{
		Reload();
	}
}

void UCombatComponent::EndStun()
{
	CombatState = ECombatState::ECS_Unoccupied;
}

void UCombatComponent::UpdateAmmoValues()
{
	if (Character == nullptr || EquipedWeapon == nullptr) return;
	int32 ReloadAmount = AmountToReload();
	if (CarriedAmmoMap.Contains(EquipedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquipedWeapon->GetWeaponType()] -= ReloadAmount;
		CarriedAmmo = CarriedAmmoMap[EquipedWeapon->GetWeaponType()];
	}
	Controller = Controller == nullptr ? 
		Cast<AMultiPlayPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
		Controller->SetHUDInventory(this);
	}

	EquipedWeapon->AddAmmo(ReloadAmount);
}

void UCombatComponent::UpdateShellAmmoValues()
{
	if (Character == nullptr || EquipedWeapon == nullptr) return;
	if (CarriedAmmoMap.Contains(EquipedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquipedWeapon->GetWeaponType()] -= 1;
		CarriedAmmo = CarriedAmmoMap[EquipedWeapon->GetWeaponType()];
	}
	Controller = Controller == nullptr ? 
		Cast<AMultiPlayPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
	bCanFire = true;
	EquipedWeapon->AddAmmo(1);
	if (EquipedWeapon->IsFull() || CarriedAmmo == 0)
	{
		JumpToShotGunEnd();
	}
}


void UCombatComponent::JumpToShotGunEnd()
{
	if (EquipedWeapon == nullptr)return;
	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (AnimInstance && Character->GetReloadMontage())
	{
		AnimInstance->Montage_JumpToSection(FName("ShotGunEnd"));
	}
}


void UCombatComponent::ThrowGrenade(EThrowType Type)
{
	if (Grenades==0 || CombatState != ECombatState::ECS_Unoccupied || EquipedWeapon==nullptr ) return;
	ThrowType = Type;
	CombatState = ECombatState::ECS_ThrowGrenadeing;
	if (Character)
	{
		Character->PlayThrowGrenadeMontage();
		AttachActorToLeftHand(EquipedWeapon);
		ShowAttachedGrenade(true);
	}

}

void UCombatComponent::FinishThrowGrenade()
{
	CombatState = ECombatState::ECS_Unoccupied;
	AttachActorToRightHand(EquipedWeapon);
	ShowAttachedGrenade(false);
}

void UCombatComponent::GrenadeLaunch()
{
	ShowAttachedGrenade(false);
	ThrowDrawFinish();
	if (Character && Character->IsLocallyControlled())
	{
		ServerLaunchGrenade(HitTarget, ThrowType);
	}

	if (Character && Character->HasAuthority())
	{
		Grenades = FMath::Clamp(Grenades - 1, 0, MaxGrenades);
		UpdateHUDGrenades();
	}
}

void UCombatComponent::ServerLaunchGrenade_Implementation(const FVector_NetQuantize& Target, EThrowType Type)
{
	if (Character && Character->HasAuthority() && GrenadeClass && Character->GetAttachedGrenade())
	{
		const FVector StartingLocation = Character->GetAttachedGrenade()->GetComponentLocation();
		FVector Forward = HitTarget-StartingLocation;
		Forward.Normalize();
		FActorSpawnParameters SpawnParam;
		SpawnParam.Owner = Character;
		SpawnParam.Instigator = Character;
		UWorld* World = GetWorld();

		if (World)
		{
			switch (Type)
			{
			case EThrowType::ETT_Grenade:
			{
				AProjectile* Grenade = World->SpawnActor<AProjectile>(
					GrenadeClass,
					StartingLocation,
					Forward.Rotation(),
					SpawnParam
					);
				break;
			}

			case EThrowType::ETT_SmokeGrenade:
			{
				AProjectile* Grenade = World->SpawnActor<AProjectile>(
					SmokeGrenadeClass,
					StartingLocation,
					Forward.Rotation(),
					SpawnParam
					);
				break;
			}

			}
			
		}
	}
}

void UCombatComponent::ServerThrowGrenade_Implementation()
{
	CombatState = ECombatState::ECS_ThrowGrenadeing;
	if (Character)
	{
		Character->PlayThrowGrenadeMontage();
		AttachActorToLeftHand(EquipedWeapon);
		ShowAttachedGrenade(true);
	}
	Grenades = FMath::Clamp(Grenades - 1, 0, MaxGrenades);
	UpdateHUDGrenades();
}

void UCombatComponent::ServerAddGrenade_Implementation(int Grenade)
{
	Grenades = FMath::Clamp(Grenades + Grenade, 0, MaxGrenades);
	UpdateHUDGrenades();
}

void UCombatComponent::OnRep_Grenades()
{
	UpdateHUDGrenades();
}

void UCombatComponent::UpdateHUDGrenades()
{
	Controller = Controller == nullptr ? Cast<AMultiPlayPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDGrenades(Grenades);
		Controller->SetHUDInventory(this);
	}
}

void UCombatComponent::AddThrowGrenades()
{
	if (Character && Character->HasAuthority())
	{
		Grenades = FMath::Clamp(Grenades + 1, 0, MaxGrenades);
		UpdateHUDGrenades();
	}
}

void UCombatComponent::ServerReload_Implementation()
{
	CombatState = ECombatState::ECS_Reloading;
	HandleReload();
}

void UCombatComponent::HandleReload()
{
	Character->PlayReloadMontage();
}

int32 UCombatComponent::AmountToReload()
{
	if (EquipedWeapon == nullptr) return 0;
	int32 Ammo = EquipedWeapon->GetAmmo();
	
	int32 RoomInMag= EquipedWeapon->GetMagCapacity()- EquipedWeapon->GetAmmo();
	if (CarriedAmmoMap.Contains(EquipedWeapon->GetWeaponType()))
	{
		int32 AmountCarried = CarriedAmmoMap[EquipedWeapon->GetWeaponType()];
		int32 Least = FMath::Min(RoomInMag, AmountCarried);
		return FMath::Clamp(RoomInMag, 0, Least);
	}
	return 0;
}


void UCombatComponent::ReadyThrow()
{
	Throwing = true;
	Character->GetWorldTimerManager().SetTimer(
		ThrowDrawTimer,
		this,
		&UCombatComponent::ThrowDraw,
		ThrowDrawTime,
		true
	);
}

void UCombatComponent::ThrowDraw()
{
	if (Throwing)
	{
		if (Character)
		{
			USplineComponent* Spline = Character->GetSplineThrowComponent();
			Spline->ClearSplinePoints();
			for (int i = 0; i < SplineMeshThrows.Num(); ++i)
			{
				SplineMeshThrows[i]->DestroyComponent();
			}
			SplineMeshThrows.Empty();


			const FVector StartingLocation = Character->GetAttachedGrenade()->GetComponentLocation();
			FVector Target = HitTarget- StartingLocation;
			Target.Normalize();
			const FVector Forward = Target;

			FPredictProjectilePathParams predictParams(20.0f, StartingLocation, Forward* 2000.f, 15.0f, ECollisionChannel::ECC_Camera
				, Character);
			predictParams.DrawDebugTime = 0.f;
			predictParams.MaxSimTime = 2.f;
			predictParams.SimFrequency = 15.f;
			FPredictProjectilePathResult result;
			UGameplayStatics::PredictProjectilePath(GetWorld(), predictParams, result);
			for (int i = 0; i < result.PathData.Num(); ++i)
			{
				Spline->AddSplinePointAtIndex(result.PathData[i].Location, i, ESplineCoordinateSpace::Local);
			}

			for (int i = 0; i < Spline->GetNumberOfSplinePoints() - 2; ++i)
			{
				USplineMeshComponent* SplineMeshComponent = Cast<USplineMeshComponent>(Character->AddComponentByClass(
					USplineMeshComponent::StaticClass(),
					true, 
					FTransform(), 
					false));
				SplineMeshComponent->SetMobility(EComponentMobility::Movable);
				if (SplineThrowDrawStaticMesh && SplineThrowDrawMaterial)
				{
					SplineMeshComponent->SetStaticMesh(SplineThrowDrawStaticMesh);
					SplineMeshComponent->SetMaterial(0, SplineThrowDrawMaterial);
				}
				Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
				SplineMeshComponent->SetStartAndEnd(
					Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local),
					Spline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local),
					Spline->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local),
					Spline->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Local)
				);
				SplineMeshThrows.Add(SplineMeshComponent);
			}

		}
	}
}

void UCombatComponent::ThrowDrawFinish()
{
	USplineComponent* Spline = Character->GetSplineThrowComponent();
	Spline->ClearSplinePoints();
	for (int i = 0; i < SplineMeshThrows.Num(); ++i)
	{
		SplineMeshThrows[i]->DestroyComponent();
	}
	SplineMeshThrows.Empty();
	Character->GetWorldTimerManager().ClearTimer(ThrowDrawTimer);
}

void UCombatComponent::OnRep_CombatState()
{
	switch (CombatState)
	{
	case ECombatState::ECS_Reloading:
		HandleReload();
		break;
	case ECombatState::ECS_Unoccupied:
		if (bFireBtnPressed)
		{
			Fire();
		}
		break;
	case ECombatState::ECS_ThrowGrenadeing:
		if (Character && !Character->IsLocallyControlled())
		{
			Character->PlayThrowGrenadeMontage();
			AttachActorToLeftHand(EquipedWeapon);
			ShowAttachedGrenade(true);
		}
		break;
	}
}

void UCombatComponent::TraceCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	FVector2D CrosshairLocation(ViewportSize.X/2.f, ViewportSize.Y/2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0), 
		CrosshairLocation, 
		CrosshairWorldPosition, 
		CrosshairWorldDirection);
	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;
		if (Character)
		{
			float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 100.f);
		}
		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(TraceHitResult, Start, End, ECollisionChannel::ECC_Visibility);
		if (!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
		}
		if (TraceHitResult.GetActor() && 
			(Cast<IInteractWithCrosshairsInterface>(TraceHitResult.GetActor())||
				Cast<IBulletHitInterface>(TraceHitResult.GetActor())))
		{
			HUDPackage.CrosshairsColor = FLinearColor::Red;
		}
		else
		{
			HUDPackage.CrosshairsColor = FLinearColor::White;
		}
	}
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (EquipedWeapon == nullptr)return;
	if (Character && CombatState == ECombatState::ECS_Reloading && EquipedWeapon->GetWeaponType() == EWeaponType::EWT_ShotGun)
	{
		Character->PlayFireMontage(bAiming);
		EquipedWeapon->Fire(TraceHitTarget);
		CombatState = ECombatState::ECS_Unoccupied;
		return;
	}
	if (Character && CombatState==ECombatState::ECS_Unoccupied)
	{
		Character->PlayFireMontage(bAiming);
		EquipedWeapon->Fire(TraceHitTarget);
	}
}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : RunSpeed;
	}
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr) return;
	if (CombatState != ECombatState::ECS_Unoccupied) return;
	if (MaxSlotIndex < INVENTORY_CAPACITY)
	{
		if (MaxSlotIndex == 0)
		{
			EquipedWeapon = WeaponToEquip;
			EquipedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
			SlotIndex = 0;
		}
		else
		{
			WeaponToEquip->SetWeaponState(EWeaponState::EWS_PickedUp);
		}
		Inventory[MaxSlotIndex]= WeaponToEquip;
		MaxSlotIndex++;
	}
	else
	{
		DropEquippedWeapon();
		Inventory[SlotIndex] = WeaponToEquip;
		EquipedWeapon = WeaponToEquip;
		EquipedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	}
	AttachActorToRightHand(WeaponToEquip);

	WeaponToEquip->SetOwner(Character);
	EquipedWeapon->SetHUDAmmo();
	UpdateCarriedAmmo();
	if (Character && EquipedWeapon && EquipedWeapon->EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, WeaponToEquip->EquipSound, 
			Character->GetActorLocation());
	}
	ReloadZeroAmmoWeapon();
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
	if (Controller)
	{
		Controller->SetHUDInventory(this);
	}
}

void UCombatComponent::LoadEquipWeapon(AWeapon* WeaponToEquip)
{
	//캐릭터와 무기장착상태를 확인합니다.
	if (Character == nullptr || WeaponToEquip == nullptr) return;
	//현재 플레이어의 상태를 확인합니다.
	if (CombatState != ECombatState::ECS_Unoccupied) return;
	//인벤토리가 꽉찼는지 확인하고, 이에대한 처리를 합니다.
	if (MaxSlotIndex < INVENTORY_CAPACITY)
	{
		if (MaxSlotIndex == 0)
		{
			EquipedWeapon = WeaponToEquip;
			EquipedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
			SlotIndex = 0;
		}
		else
		{

			WeaponToEquip->SetWeaponState(EWeaponState::EWS_PickedUp);
		}
		Inventory[MaxSlotIndex] = WeaponToEquip;
		MaxSlotIndex++;
	}
	else
	{
		//인벤토리가 꽉차있다면 현재 가지고있는 무기를 버리는 함수입니다.
		DropEquippedWeapon();
		Inventory[SlotIndex] = WeaponToEquip;
		EquipedWeapon = WeaponToEquip;
		EquipedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	}
	//오른손을 소켓에 맞춰주는 함수입니다.
	AttachActorToRightHand(WeaponToEquip);

	WeaponToEquip->SetOwner(Character);

	EquipedWeapon->SetHUDAmmo();

	UpdateCarriedAmmo();
	//무기총알이 0일경우 재장전을 호출해주는 함수입니다.
	ReloadZeroAmmoWeapon();
	//입력방향으로 회전을 막아주도록 하였습니다.
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;

	if (Controller)
	{
		//인벤토리 업데이트입니다.
		Controller->SetHUDInventory(this);
	}
}

void UCombatComponent::SwapWeapon(int32 Index)
{
	//1이상 입력받고있음
	Index -= 1;
	if (Character == nullptr) return;
	if (CombatState != ECombatState::ECS_Unoccupied) return;
	if (Index >= INVENTORY_CAPACITY) return;
	if (Inventory[Index] == nullptr) return;

	if (Inventory[SlotIndex])
	{
		Inventory[SlotIndex]->SetWeaponState(EWeaponState::EWS_PickedUp);
	}
	SlotIndex = Index;
	EquipedWeapon = Inventory[Index];

	AttachActorToRightHand(EquipedWeapon);
	EquipedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	EquipedWeapon->SetHUDAmmo();
	UpdateCarriedAmmo();
	PlayEquipWeaponSound();
	ReloadZeroAmmoWeapon();
	Controller->SetHUDInventory(this);
}



void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquipedWeapon && Character)
	{
	}
}

void UCombatComponent::DropEquippedWeapon()
{
	if (EquipedWeapon)
	{
		EquipedWeapon->Dropped();
	}
}

void UCombatComponent::AttachActorToLeftHand(AActor* ActorToAttach)
{
	if (Character == nullptr || Character->GetMesh() == nullptr || ActorToAttach == nullptr || EquipedWeapon==nullptr) return;
	bool bUsePistolSocket = 
		EquipedWeapon->GetWeaponType() == EWeaponType::EWT_Pistol || 
		EquipedWeapon->GetWeaponType() == EWeaponType::EWT_SMG;
	FName SocketName = bUsePistolSocket ? FName("PistolSocket") : FName("LeftHandSocket");
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(SocketName);
	if (HandSocket)
	{
		HandSocket->AttachActor(EquipedWeapon, Character->GetMesh());
	}
}

void UCombatComponent::AttachActorToRightHand(AActor* ActorToAttach)
{
	if (Character == nullptr || Character->GetMesh()==nullptr || ActorToAttach == nullptr) return;
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (HandSocket)
	{
		HandSocket->AttachActor(EquipedWeapon, Character->GetMesh());
	}
}

void UCombatComponent::AttachActorToBack(AActor* ActorToAttach)
{
	if (Character == nullptr || Character->GetMesh() == nullptr || ActorToAttach == nullptr) return;
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("BackpackSocket"));
	if (HandSocket)
	{
		HandSocket->AttachActor(EquipedWeapon, Character->GetMesh());
	}
}

void UCombatComponent::UpdateCarriedAmmo()
{
	if (EquipedWeapon == nullptr) return;
	if (CarriedAmmoMap.Contains(EquipedWeapon->GetWeaponType()))
	{
		CarriedAmmo = CarriedAmmoMap[EquipedWeapon->GetWeaponType()];
	}
	Controller = Controller == nullptr ? Cast<AMultiPlayPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
		Controller->SetHUDInventory(this);
	}
}

void UCombatComponent::PlayEquipWeaponSound()
{
	
	if (Character && EquipedWeapon && EquipedWeapon->EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EquipedWeapon->EquipSound, Character->GetActorLocation());
	}
}

void UCombatComponent::ReloadZeroAmmoWeapon()
{
	if (EquipedWeapon && !EquipedWeapon->IsAmmo())
	{
		Reload();
	}
}

void UCombatComponent::ShowAttachedGrenade(bool bShowGrenade)
{
	if (Character && Character->GetAttachedGrenade())
	{
		Character->GetAttachedGrenade()->SetVisibility(bShowGrenade);
	}
}

