// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiPlayCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "UE5_MultiPlayFPS/Weapon/Weapon.h"
#include "UE5_MultiPlayFPS/MultiPlayComponents/CombatComponent.h"
#include "UE5_MultiPlayFPS/MultiPlayComponents/BuffComponent.h"
#include "UE5_MultiPlayFPS/MultiPlayComponents/ParkourComponent.h"
#include "UE5_MultiPlayFPS/MultiPlayComponents/AbilityComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MultiPlayAnimInstance.h"
#include "UE5_MultiPlayFPS/UE5_MultiPlayFPS.h"
#include "UE5_MultiPlayFPS/PlayerController/MultiPlayPlayerController.h"
#include "UE5_MultiPlayFPS/GameMode/MultiPlayGameMode.h"
#include "TimerManager.h"
#include "UE5_MultiPlayFPS/PlayerState/MultiPlayPlayerState.h"
#include "UE5_MultiPlayFPS/Weapon/WeaponTypes.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"	
#include "Sound/SoundCue.h"
#include "UE5_MultiPlayFPS/AI/EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "UE5_MultiPlayFPS/Widget/HUD/DamageIndicatorWidget.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Components/SplineComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
AMultiPlayCharacter::AMultiPlayCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	m_Arm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Arm"));

	m_Arm->SetupAttachment(GetMesh());
	m_Arm->TargetArmLength = 600.f;
	m_Arm->bUsePawnControlRotation = true;

	m_Cam = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	m_Cam->SetupAttachment(m_Arm, USpringArmComponent::SocketName);
	m_Cam->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	SplineThrowComponent = CreateDefaultSubobject<USplineComponent >(TEXT("SplineThrowComponent"));
	SplineThrowComponent->SetupAttachment(RootComponent);

	Combat1 = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent2"));
	Combat1->SetIsReplicated(true);

	Buff= CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	Buff->SetIsReplicated(true);

	Parkour1 = CreateDefaultSubobject<UParkourComponent>(TEXT("ParkourComponent1"));
	Parkour1->SetIsReplicated(true);

	Ability = CreateDefaultSubobject<UAbilityComponent>(TEXT("AbilityComponent"));
	Ability->SetIsReplicated(true);

	AIStimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("AIStimuliSourceComponent"));

	AttachedGrenade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Attached Grenade"));
	AttachedGrenade->SetupAttachment(GetMesh(), FName("GrenadeSocket"));
	AttachedGrenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DissolveTimeLine = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Block);
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 900.f);
	HandSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HandSceneComp"));

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;

	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;
	CameraThreshold = 200.f;
	TurnThreshold = 0.5f;
	bElimed = false;
	bDisableGameplay = false;
	MaxShield = 100.f;
	Shield = 100.f;
	StunChance = 0.25f;
	bShop = false;
}

void AMultiPlayCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME_CONDITION(AMultiPlayCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(AMultiPlayCharacter, Health);
	DOREPLIFETIME(AMultiPlayCharacter, Shield);
	DOREPLIFETIME(AMultiPlayCharacter, bDisableGameplay);
}

void AMultiPlayCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat1)
	{
		Combat1->Character = this;
	}
	if (Buff)
	{
		Buff->Character = this;
	}

	if (Parkour1)
	{
		Parkour1->Character = this;
	}

	if (Ability)
	{
		Ability->Character = this;
	}
}

void AMultiPlayCharacter::Destroyed()
{
	Super::Destroyed();
	AMultiPlayGameMode* PlayGameMode = Cast<AMultiPlayGameMode>(UGameplayStatics::GetGameMode(this));
	bool MatchNotInProgress = PlayGameMode && PlayGameMode->GetMatchState() != MatchState::InProgress;
	

	if (Combat1 && Combat1->EquipedWeapon && MatchNotInProgress)
	{
		Combat1->EquipedWeapon->Destroy();
	}
}

void AMultiPlayCharacter::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
	BaseMaterial = GetMesh()->GetMaterial(0)->GetMaterial();
	//if (HasAuthority())
	//{
		//UPROPERTY(BlueprintAssignable, Category = "Game|Damage")
		//	FTakeAnyDamageSignature OnTakeAnyDamage;
		OnTakeAnyDamage.AddDynamic(this, &AMultiPlayCharacter::ReceiveDamage);
	//}

	if (AttachedGrenade)
	{
		AttachedGrenade->SetVisibility(false);
	}


}

void AMultiPlayCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RotateInPlace(DeltaTime);
	HideCameraToCharacter();
	PollInit();
}


void AMultiPlayCharacter::RotateInPlace(float DeltaTime)
{
	if (bDisableGameplay)
	{
		bUseControllerRotationYaw = false;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AImOffset(DeltaTime);
	}
	else
	{
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}
		CalCulateAimOffset_Pitch();
	}
}

void AMultiPlayCharacter::SpawnDefaultWeapon()
{
	if (DefaultWeaponClass)
	{
		AWeapon* DefaultWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
		if (Combat1)
		{
			Combat1->EquipWeapon(DefaultWeapon);
		}
	}
	
}
void AMultiPlayCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AMultiPlayCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AMultiPlayCharacter::MoveRight);

	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AMultiPlayCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("TurnRight"), this, &AMultiPlayCharacter::TurnRight);
	//PlayerInputComponent->BindAxis(TEXT("Zoom"), this, &AMyCharacter::Zoom);
	
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &AMultiPlayCharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Equip"), EInputEvent::IE_Pressed, this, &AMultiPlayCharacter::EquipBtnPressed);
	PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Pressed, this, &AMultiPlayCharacter::CrouchBtnPressed);
	PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Released, this, &AMultiPlayCharacter::CrouchBtnReleased);
	PlayerInputComponent->BindAction(TEXT("Aim"), EInputEvent::IE_Pressed, this, &AMultiPlayCharacter::AimBtnPressed);
	PlayerInputComponent->BindAction(TEXT("Aim"), EInputEvent::IE_Released, this, &AMultiPlayCharacter::AimBtnReleased);

	PlayerInputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Pressed, this, &AMultiPlayCharacter::FireBtnPressed);
	PlayerInputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Released, this, &AMultiPlayCharacter::FireBtnReleased);
	PlayerInputComponent->BindAction(TEXT("Reload"), EInputEvent::IE_Pressed, this, &AMultiPlayCharacter::ReloadBtnPressed);
	PlayerInputComponent->BindAction(TEXT("ThrowGrenade"), EInputEvent::IE_Pressed, this, &AMultiPlayCharacter::GrenadeBtnPressed);
	PlayerInputComponent->BindAction(TEXT("ThrowGrenade"), EInputEvent::IE_Released, this, &AMultiPlayCharacter::GrenadeBtnReleased);
	PlayerInputComponent->BindAction(TEXT("ThrowSmokeGrenade"), EInputEvent::IE_Pressed, this, &AMultiPlayCharacter::SmokeGrenadeBtnPressed);
	PlayerInputComponent->BindAction(TEXT("ThrowSmokeGrenade"), EInputEvent::IE_Released, this, &AMultiPlayCharacter::GrenadeBtnReleased);
	
	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Pressed, this, &AMultiPlayCharacter::SprintBtnPressed);
	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Released, this, &AMultiPlayCharacter::SprintBtnReleased);

	PlayerInputComponent->BindAction(TEXT("SwapWeapon1"), EInputEvent::IE_Pressed, this, &AMultiPlayCharacter::SwapWeaponBtn1Pressed);
	PlayerInputComponent->BindAction(TEXT("SwapWeapon2"), EInputEvent::IE_Pressed, this, &AMultiPlayCharacter::SwapWeaponBtn2Pressed);
	PlayerInputComponent->BindAction(TEXT("SwapWeapon3"), EInputEvent::IE_Pressed, this, &AMultiPlayCharacter::SwapWeaponBtn3Pressed);
	PlayerInputComponent->BindAction(TEXT("SwapWeapon4"), EInputEvent::IE_Pressed, this, &AMultiPlayCharacter::SwapWeaponBtn4Pressed);
	PlayerInputComponent->BindAction(TEXT("SwapWeapon5"), EInputEvent::IE_Pressed, this, &AMultiPlayCharacter::SwapWeaponBtn5Pressed);

	PlayerInputComponent->BindAction(TEXT("EnemyScan"), EInputEvent::IE_Pressed, this, &AMultiPlayCharacter::ScanBtnPressed);
	PlayerInputComponent->BindAction(TEXT("CreatePortal"), EInputEvent::IE_Pressed, this, &AMultiPlayCharacter::CreatePortalBtnPressed);
	PlayerInputComponent->BindAction(TEXT("ShopOnOff"), EInputEvent::IE_Pressed, this, &AMultiPlayCharacter::ShopOnOffBtnPressed);
	//ThrowSmokeGrenade
}

void AMultiPlayCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat1 == nullptr || Combat1->EquipedWeapon==nullptr)return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AMultiPlayCharacter::PlayReloadMontage()
{
	if (Combat1 == nullptr || Combat1->EquipedWeapon == nullptr)return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName;
		switch (Combat1->EquipedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			SectionName = FName("Rifle");
			break;

		case EWeaponType::EWT_RocketLauncher:
			SectionName = FName("RocketLauncher");
			break;

		case EWeaponType::EWT_Pistol:
			SectionName = FName("Pistol");
			break;

		case EWeaponType::EWT_SMG:
			SectionName = FName("Pistol");
			break;

		case EWeaponType::EWT_ShotGun:
			SectionName = FName("ShotGun");
			break;

		case EWeaponType::EWT_SniperRifle:
			SectionName = FName("SniperRifle");
			break;

		case EWeaponType::EWT_GrenadeLauncher:
			SectionName = FName("GrenadeLauncher");
			break;
		}

		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AMultiPlayCharacter::PlayElimMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ElimMontage)
	{
		AnimInstance->Montage_Play(ElimMontage);
	}
}

void AMultiPlayCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	if (GetLocalRole() == ENetRole::ROLE_SimulatedProxy)
	{
		SimProxiesTurn();
	}
	TimeSinceLastMovementReplication = 0.f;
}

void AMultiPlayCharacter::Elim()
{
	if (Combat1 && Combat1->EquipedWeapon)
	{
		Combat1->EquipedWeapon->Dropped();
	}
	Multicast_Elim();
	GetWorldTimerManager().SetTimer(ElimTimer, this, &AMultiPlayCharacter::ElimTimerFinished, ElimDelay);
}

void AMultiPlayCharacter::Multicast_Elim_Implementation()
{
	if (MultiPlayPlayerController)
	{
		MultiPlayPlayerController->SetHUDWeaponAmmo(0);
	}
	bElimed = true;

	GetMesh()->bPauseAnims = true;
	GetMesh()->SetSimulatePhysics(true);
	//PlayElimMontage();


	if (DissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);

		GetMesh()->SetMaterial(0, DynamicDissolveMaterialInstance);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), 0.55f);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Glow"), 200.f);
	}
	if (DissolveMaterialFaceInstance)
	{
		DynamicPaperBurnMaterialFaceInstance = UMaterialInstanceDynamic::Create(DissolveMaterialFaceInstance, this);

		GetMesh()->SetMaterial(1, DynamicPaperBurnMaterialFaceInstance);
		DynamicPaperBurnMaterialFaceInstance->SetScalarParameterValue(TEXT("Dissolve"), 0.55f);
	}
	StartDissolve();

	//Stop Movement

	bDisableGameplay = true;
	GetCharacterMovement()->DisableMovement();

	if (Combat1)
	{
		Combat1->FireBtnPressed(false);
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bool bSniperScopeHide= IsLocallyControlled() && Combat1 && Combat1->bAiming && Combat1->EquipedWeapon && Combat1->EquipedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle;
	if (bSniperScopeHide)
	{
		ShowSniperScopeWidget(false);
	}

}
void AMultiPlayCharacter::ElimTimerFinished()
{
	AMultiPlayGameMode* PlayGameMode = GetWorld()->GetAuthGameMode<AMultiPlayGameMode>();
	if (PlayGameMode)
	{
		PlayGameMode->RequestRespawn(this, Controller);
		
	}
}

void AMultiPlayCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	if (DynamicDissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	}
	if (DynamicPaperBurnMaterialFaceInstance)
	{
		DynamicPaperBurnMaterialFaceInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	}
}

void AMultiPlayCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &AMultiPlayCharacter::UpdateDissolveMaterial);
	if (DissolveCurve && DissolveTimeLine)
	{
		DissolveTimeLine->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeLine->Play();
	}
}

void AMultiPlayCharacter::PlayHitReactMontage()
{
	if (Combat1 == nullptr || Combat1->EquipedWeapon == nullptr)return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName;
		SectionName = FName("FrontHit");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AMultiPlayCharacter::Stun()
{
	if (Combat1 == nullptr) return;
	Combat1->CombatState = ECombatState::ECS_Stunned;
	PlayHitReactMontage();
}

void AMultiPlayCharacter::CreateDamageIndicatorHUD(ACharacter* HitCharacter, FVector& HitLocation)
{
	if (DamageIndicatorWidget)
	{
		UDamageIndicatorWidget* Widget=CreateWidget<UDamageIndicatorWidget>(GetWorld(), DamageIndicatorWidget, "DamageWidget");
		if (Widget)
		{
			Widget->AddToViewport();
			Widget->SetInit(HitCharacter, HitLocation);
		}
	}
}

void AMultiPlayCharacter::PlayThrowGrenadeMontage(FName Selection)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ThrowMontage)
	{
		AnimInstance->Montage_Play(ThrowMontage);
		AnimInstance->Montage_JumpToSection(Selection);
	}
}

void AMultiPlayCharacter::ReceiveDamage(AActor* DamageActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser)
{
	if (bElimed) return;
	if (Shield > 0.f)
	{
		if (Shield >= Damage)
		{
			Shield = FMath::Clamp(Shield - Damage, 0.f, MaxShield);
			Damage = 0.f;
		}
		else
		{
			Damage = FMath::Clamp(Damage - Shield, 0.f, Damage);
			Shield = 0.f;
		}
	}
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	if (DamageCauser)
	{
		FVector HitLocation = DamageCauser->GetActorLocation();
		CreateDamageIndicatorHUD(this, HitLocation);
	}
	UpdateHUDHealth();
	UpdateHUDShield();
	if (Health == 0.f)
	{
		AEnemyAIController* EnemyController = Cast<AEnemyAIController>(InstigatorController);
		if (EnemyController)
		{
			EnemyController->GetBlackboardComponent()->SetValueAsBool(FName(TEXT("CharacterDead")), true);
		}
		
		AMultiPlayGameMode* PlayGameMode = GetWorld()->GetAuthGameMode<AMultiPlayGameMode>();
		if (PlayGameMode)
		{
			MultiPlayPlayerController = MultiPlayPlayerController == nullptr ? Cast<AMultiPlayPlayerController>(Controller) : MultiPlayPlayerController;
			AMultiPlayPlayerController* AttackerController = Cast<AMultiPlayPlayerController>(InstigatorController);
			PlayGameMode->PlayerEliminated(this, MultiPlayPlayerController, AttackerController);
		}
	}
}

void AMultiPlayCharacter::MoveForward(float Value)
{
	if (bDisableGameplay) return;
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);

		GetActorForwardVector();
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
	}
}

void AMultiPlayCharacter::MoveRight(float Value)
{
	if (bDisableGameplay) return;
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);

		GetActorForwardVector();
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Value);
	}
}

void AMultiPlayCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AMultiPlayCharacter::TurnRight(float Value)
{
	AddControllerYawInput(Value);
}

void AMultiPlayCharacter::ZoomIn()
{

}

void AMultiPlayCharacter::EquipBtnPressed()
{
	if (bDisableGameplay) return;
	if (Combat1)
	{
		if (HasAuthority())
		{
			if (OverlappingItem)
			{
				if (OverlappingItem->ItemType == EItemType::EIT_Weapon)
					Combat1->EquipWeapon(Cast<AWeapon>(OverlappingItem));

				else
					OverlappingItem->EquippedEvent(this);
				
			}
		}
		else
			ServerEquipButtonPressed();
	}
}

void AMultiPlayCharacter::CrouchBtnPressed()
{
	if (bDisableGameplay) return;
	Crouch();
	if (Parkour1)
	{
		Parkour1->SlideEvent();
	}
}

void AMultiPlayCharacter::CrouchBtnReleased()
{
	if (bDisableGameplay) return;
	UnCrouch();
	
}

void AMultiPlayCharacter::SprintBtnPressed()
{
	if (Parkour1)
	{
		Parkour1->SprintEvent();
	}
}

void AMultiPlayCharacter::SprintBtnReleased()
{
	if (Parkour1)
	{
		Parkour1->SprintEnd(0.35f);
	}
}

void AMultiPlayCharacter::ReloadBtnPressed()
{
	if (bDisableGameplay) return;
	if (Combat1)
	{
		Combat1->Reload();
	}
}

void AMultiPlayCharacter::AimBtnPressed()
{
	if (bDisableGameplay) return;
	if (Combat1)
	{
		Combat1->SetAiming(true);
	}
}

void AMultiPlayCharacter::AimBtnReleased()
{
	if (bDisableGameplay) return;
	if (Combat1)
	{
		Combat1->SetAiming(false);
	}
}

void AMultiPlayCharacter::SwapWeaponBtn1Pressed()
{
	if (Combat1)
	{
		Combat1->SwapWeapon(1);
	}
}

void AMultiPlayCharacter::SwapWeaponBtn2Pressed()
{
	if (Combat1)
	{
		Combat1->SwapWeapon(2);
	}
}

void AMultiPlayCharacter::SwapWeaponBtn3Pressed()
{
	if (Combat1)
	{
		Combat1->SwapWeapon(3);
	}
}

void AMultiPlayCharacter::SwapWeaponBtn4Pressed()
{
	if (Combat1)
	{
		Combat1->SwapWeapon(4);
	}
}

void AMultiPlayCharacter::SwapWeaponBtn5Pressed()
{
	if (Combat1)
	{
		Combat1->SwapWeapon(5);
	}
}

void AMultiPlayCharacter::ScanBtnPressed()
{
	if (Ability)
	{
		Ability->ScanEnemy();
	}
}

void AMultiPlayCharacter::AImOffset(float DT)
{
	if (Combat1 && Combat1->EquipedWeapon == nullptr) return;

	float Speed = CalculateSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.f && !bIsInAir)
	{
		bRotateRootBone = true;
		FRotator CurrentAimRot = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRot = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRot, StartAimRot);
		AimOffset_Yaw = DeltaAimRot.Yaw;
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAimOffset_Yaw = AimOffset_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DT);
	}
	if (Speed > 0.f || bIsInAir)
	{
		bRotateRootBone = false;
		StartAimRot = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AimOffset_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}
	//0~360범위에 부호X Short로 Pitch는 Short로 만들어져있음
	// CompressAxisToShort
	//음수값은 양수로 바뀜
	//270~360  을 -90~0으로

	CalCulateAimOffset_Pitch();

}

void AMultiPlayCharacter::CalCulateAimOffset_Pitch()
{
	AimOffset_Pitch = GetBaseAimRotation().Pitch;
	if (AimOffset_Pitch > 90.f && !IsLocallyControlled())
	{
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AimOffset_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AimOffset_Pitch);
	}
}

void AMultiPlayCharacter::SimProxiesTurn()
{
	if (Combat1 == nullptr || Combat1->EquipedWeapon == nullptr)return;
	bRotateRootBone = false;
	float Speed = CalculateSpeed();
	if (Speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}

	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw =UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;

	if (FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		if (ProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if (ProxyYaw < -TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
}

void AMultiPlayCharacter::Jump()
{
	if (bDisableGameplay) return;
	UnCrouch();
	Super::Jump();
	if (Parkour1)
	{

		Parkour1->JumpEvent();
	}
}

void AMultiPlayCharacter::FireBtnPressed()
{
	if (bDisableGameplay || bShop) return;
	if (Combat1)
	{
		Combat1->FireBtnPressed(true);
	}
}

void AMultiPlayCharacter::FireBtnReleased()
{
	if (bDisableGameplay) return;
	if (Combat1)
	{
		Combat1->FireBtnPressed(false);
	}
}

void AMultiPlayCharacter::GrenadeBtnPressed()
{
	if (Combat1)
	{
		Combat1->ThrowGrenade(EThrowType::ETT_Grenade);
	}
}

void AMultiPlayCharacter::GrenadeBtnReleased()
{
	if (Combat1)
	{
		if (Combat1->GetThrowing())
		{
			PlayThrowGrenadeMontage(FName(TEXT("End")));
			Combat1->SetThrowing(false);
			Combat1->GrenadeLaunch();
		}
		else
		{
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance && ThrowMontage)
			{
				AnimInstance->Montage_Stop(0.f, ThrowMontage);
				Combat1->FinishThrowGrenade();
			}
		}
	}
}

void AMultiPlayCharacter::TurnInPlace(float DT)
{
	if (AimOffset_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AimOffset_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAimOffset_Yaw = FMath::FInterpTo(InterpAimOffset_Yaw, 0.f, DT, 10.f);
		AimOffset_Yaw = InterpAimOffset_Yaw;
		if (FMath::Abs(AimOffset_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartAimRot = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

void AMultiPlayCharacter::HideCameraToCharacter()
{
	if (!IsLocallyControlled())return;
	if ((m_Cam->GetComponentLocation() - GetActorLocation()).Size()< CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if (Combat1 && Combat1->EquipedWeapon && Combat1->EquipedWeapon->GetWeaponMesh())
		{
			Combat1->EquipedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if (Combat1 && Combat1->EquipedWeapon && Combat1->EquipedWeapon->GetWeaponMesh())
		{
			Combat1->EquipedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

void AMultiPlayCharacter::CreatePortalBtnPressed()
{
	if (Ability)
	{
		Ability->CreatePortal();
	}
}

void AMultiPlayCharacter::SmokeGrenadeBtnPressed()
{
	if (Combat1)
	{
		Combat1->ThrowGrenade(EThrowType::ETT_SmokeGrenade);
	}
}

void AMultiPlayCharacter::ShopOnOffBtnPressed()
{
	MultiPlayPlayerController = MultiPlayPlayerController == nullptr ? Cast<AMultiPlayPlayerController>(Controller) : MultiPlayPlayerController;
	if (MultiPlayPlayerController == nullptr) return;
	if (bShop)
	{
		bShop = false;
		MultiPlayPlayerController->ShopClose();
	}
	else
	{
		bShop = true;
		MultiPlayPlayerController->ShopOpen();
	}
}

void AMultiPlayCharacter::PortalJump(USplineComponent* Spline, bool Reverse)
{
	if (!Spline) return;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	PortalDistance = 0.f;
	PortalSpline = Spline;
	bPortalReverse = Reverse;
	if (bPortalReverse)
	{
		PortalDistance = PortalSpline->GetSplineLength();
	}
	if (PortalMaterial)
	{
		GetMesh()->SetMaterial(0, PortalMaterial);
	}
	PortalMoveComponentTo();

}

void AMultiPlayCharacter::PortalMoveComponentTo()
{
	FTransform SplineTransform = PortalSpline->GetTransformAtDistanceAlongSpline(
		PortalDistance, 
		ESplineCoordinateSpace::World);
	bool Check = bPortalReverse ? 
		PortalDistance > 0.f : PortalDistance < PortalSpline->GetSplineLength();

	FRotator Rot = SplineTransform.GetRotation().Rotator();
	if (bPortalReverse)
		Rot.Yaw += 180.f;
	if (Check)
	{
		FLatentActionInfo Info;
		Info.CallbackTarget = this;
		Info.ExecutionFunction = FName("PortalMoveComponentTo");
		Info.UUID = 2;
		Info.Linkage = 0;
		UKismetSystemLibrary::MoveComponentTo(
			GetCapsuleComponent(),
			SplineTransform.GetLocation(),
			Rot,
			false,
			false,
			0.2f,
			false,
			EMoveComponentAction::Type::Move,
			Info);
		if (bPortalReverse)
			PortalDistance = PortalDistance - 500.f;
		
		else
			PortalDistance = PortalDistance + 500.f;
	}
	else
	{
		FLatentActionInfo Info;
		Info.CallbackTarget = this;
		Info.UUID = 2;
		Info.ExecutionFunction = FName("PortalMoveFinish");
		Info.Linkage = 0;

		UKismetSystemLibrary::MoveComponentTo(
			GetCapsuleComponent(),
			SplineTransform.GetLocation(),
			Rot,
			false,
			false,
			0.2f,
			false,
			EMoveComponentAction::Type::Move,
			Info);
	}

}

void AMultiPlayCharacter::PortalMoveFinish()
{
	if (BaseMaterial)
	{
		GetMesh()->SetMaterial(0, BaseMaterial);
	}
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	LaunchCharacter(GetActorForwardVector() * 100.f, false, false);
	PortalSpline = nullptr;
}


float AMultiPlayCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();
}

void AMultiPlayCharacter::OnRep_Health(float LastHealth)
{
	UpdateHUDHealth();
	
	if (Health < LastHealth)
	{
		PlayHitReactMontage();
	}
}

void AMultiPlayCharacter::UpdateHUDHealth()
{
	MultiPlayPlayerController = MultiPlayPlayerController == nullptr ? Cast<AMultiPlayPlayerController>(Controller) : MultiPlayPlayerController;
	if (MultiPlayPlayerController)
	{
		MultiPlayPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

void AMultiPlayCharacter::OnRep_Shield(float LastShield)
{
	UpdateHUDShield();

	if (Shield < LastShield)
	{
		PlayHitReactMontage();
	}
}

void AMultiPlayCharacter::UpdateHUDShield()
{
	MultiPlayPlayerController = MultiPlayPlayerController == nullptr ? Cast<AMultiPlayPlayerController>(Controller) : MultiPlayPlayerController;
	if (MultiPlayPlayerController)
	{
		MultiPlayPlayerController->SetHUDShield(Shield, MaxShield);
	}
}


void AMultiPlayCharacter::UpdateInventoryAndAmmo()
{
	MultiPlayPlayerController = MultiPlayPlayerController == nullptr ? Cast<AMultiPlayPlayerController>(Controller) : MultiPlayPlayerController;
	if (MultiPlayPlayerController)
	{
		MultiPlayPlayerController->SetHUDCarriedAmmo(Combat1->CarriedAmmo);
		MultiPlayPlayerController->SetHUDWeaponAmmo(Combat1->EquipedWeapon->GetAmmo());
		MultiPlayPlayerController->SetHUDInventory(Combat1);
	}
}

void AMultiPlayCharacter::PollInit()
{
	if (MultiPlayPlayerState == nullptr)
	{
		MultiPlayPlayerState = GetPlayerState<AMultiPlayPlayerState>();
		if (MultiPlayPlayerState)
		{
			MultiPlayPlayerState->AddToScore(0.f);
			MultiPlayPlayerState->AddToDefeats(0);
		}
	}
	if (MultiPlayPlayerController == nullptr)
	{
		MultiPlayPlayerController = MultiPlayPlayerController == nullptr ? Cast<AMultiPlayPlayerController>(Controller) : MultiPlayPlayerController;
		if (MultiPlayPlayerController)
		{
			//SpawnDefaultWeapon();
			//UpdateInventoryAndAmmo();
			UpdateHUDHealth();
			UpdateHUDShield();
			Ability->Controller = MultiPlayPlayerController;
		}
	}
}

void AMultiPlayCharacter::ServerEquipButtonPressed_Implementation()
{
}

void AMultiPlayCharacter::SetOverlappingItem(IPickupItemInterface* Item)
{
	if (OverlappingItem)
	{
		OverlappingItem->ShowPickupWidget(false);
	}
	OverlappingItem = Item;
	//본인처리
	if (IsLocallyControlled())
	{
		if (OverlappingItem)
		{
			OverlappingItem->ShowPickupWidget(true);
		}
	}
}

bool AMultiPlayCharacter::IsWeaponEquipped()
{
	return (Combat1 && Combat1->EquipedWeapon);
}

bool AMultiPlayCharacter::IsAiming()
{
	return (Combat1 && Combat1->bAiming);
}

AWeapon* AMultiPlayCharacter::GetEquippedWeapon()
{
	if (Combat1 == nullptr) return nullptr;
	return Combat1->EquipedWeapon;
}

FVector AMultiPlayCharacter::GetHitTarget() const
{
	if (Combat1 == nullptr) return FVector();
	return Combat1->HitTarget;
}

ECombatState AMultiPlayCharacter::GetCombatState() const
{
	if (Combat1 == nullptr) return ECombatState::ECS_END;
	return Combat1->CombatState;
}

void AMultiPlayCharacter::StartCameraShakeBoom(float Scale)
{
	if (MultiPlayPlayerController && CameraBoomShake)
	{
		MultiPlayPlayerController->ClientStartCameraShake(CameraBoomShake, 1.f);
	}
}

void AMultiPlayCharacter::SetZipline(bool Enable, const FVector& Start, const FVector& End)
{
	if (Parkour1)
	{
		Parkour1->SetZipline(Enable, Start, End);
	}
}

