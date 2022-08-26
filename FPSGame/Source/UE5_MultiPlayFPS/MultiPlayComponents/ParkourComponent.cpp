#include "ParkourComponent.h"
#include "UE5_MultiPlayFPS/PlayerController/MultiPlayPlayerController.h"
#include "UE5_MultiPlayFPS/Character/MultiPlayCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameState.h"
#include "Components/CapsuleComponent.h"
#include "UE5_MultiPlayFPS/MultiPlayComponents/CombatComponent.h"

UParkourComponent::UParkourComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	WallRunSpeed = 850.f;
	WallRunJumpHeight = 300.f;
	MantleHeight = 0.f;
	ParkourType = EParkourType::EPT_NONE;
	WallRunJumpOffForce = 300.f;
	VerticalWallRunSpeed = 300.f;
	SprintSpeed = 1000.f;
	SlideImpulseAmount = 600.f;
	bVerticalWallRun = true;
	ZiplineSpeed = 500.f;
	ZiplineEnable = false;
	ZiplineStart = FVector();
	ZiplineEnd = FVector();
}


void UParkourComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UParkourComponent, ParkourType);
	DOREPLIFETIME(UParkourComponent, PrevParkourType);
}
void UParkourComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CameraTick();
	UpdateEvent();
	WallUpdateDelay -= DeltaTime;
	VerticalWallRunDelay -= DeltaTime;
	MantleUpdateDelay -= DeltaTime;
	SprintUpdateDelay -= DeltaTime;
	SLideUpdateDelay -= DeltaTime;
}


void UParkourComponent::ParkourUpdate()
{
	if (Character == nullptr) return;
	if (ParkourType == EParkourType::EPT_WALL_VERTICALRUN || 
		ParkourType == EParkourType::EPT_WALL_WALLGRAB ||
		ParkourType == EParkourType::EPT_WALL_MANTLE ||
		ParkourType == EParkourType::EPT_ZIPLINE)
	{
		if (Character->GetEquippedWeapon())
		{
			Character->GetCombat()->AttachActorToBack(Character->GetEquippedWeapon());
		}
	}
	else
	{
		if (Character->GetEquippedWeapon())
		{
			Character->GetCombat()->AttachActorToRightHand(Character->GetEquippedWeapon());
		}
	}
}

void UParkourComponent::BeginPlay()
{
	Super::BeginPlay();
	if (Character == nullptr) return;
	DefaultGravity = Character->GetCharacterMovement()->GravityScale;
	//if (Character->IsLocallyControlled())
	//{
		//Character->GetWorldTimerManager().SetTimer(
		//	WallRunTimer,
		//	this,
		//	&UParkourComponent::UpdateEvent,
		//	0.02f, true
		//);
	//}
	BaseGroundFriction = Character->GetCharacterMovement()->GroundFriction;
	BaseBrakingDecelerationWalking = Character->GetCharacterMovement()->BrakingDecelerationWalking;
	BaseMaxWalkSpeed = Character->GetCharacterMovement()->MaxWalkSpeed;
	BaseMaxCrouchWalkSpeed = Character->GetCharacterMovement()->MaxWalkSpeedCrouched;
}
void UParkourComponent::JumpEvent()
{
	if (ParkourType == EParkourType::EPT_ZIPLINE)
	{
		FLatentActionInfo Info;
		Info.CallbackTarget = this;
		Info.UUID = 0;
		Info.Linkage = 0;
		Info.ExecutionFunction = FName("ZiplineFinish");

		UKismetSystemLibrary::MoveComponentTo(
			Character->GetCapsuleComponent(),
			Character->GetActorLocation(),
			Character->GetActorRotation(),
			false,
			false,
			0.001f,
			false,
			EMoveComponentAction::Type::Move,
			Info);

		return;
	}

	if (ZiplineEnable)
	{
		ZiplinePlayStart = ZiplineStart;
		ZiplinePlayEnd = ZiplineEnd;
		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		FVector Start = ZiplinePlayStart - (FVector(0.f, 0.f, 40.f));

		FRotator Rot=UKismetMathLibrary::FindLookAtRotation(Start, ZiplineEnd);
		FLatentActionInfo Info;
		Info.CallbackTarget = this;
		Info.ExecutionFunction = FName("ZiplinePlay");
		Info.UUID = 0;
		Info.Linkage = 0;
		UKismetSystemLibrary::MoveComponentTo(
			Character->GetCapsuleComponent(),
			Start,
			Rot,
			false,
			false,
			0.2f,
			false,
			EMoveComponentAction::Type::Move,
			Info);
		
		SetParkourType(EParkourType::EPT_ZIPLINE);
		return;
	}


	if (ParkourType == EParkourType::EPT_NONE)
	{

	}
	else
	{
		WallRunJump();
		LedgeGrabJump();
		SprintJump();
		SlideJump();
	}
}

void UParkourComponent::LandEvent()
{
	WallRunEnd(0.f);
	VerticalWallRunEnd(0.f);
	SprintEnd(0.f);
	SlideEnd();
}

bool UParkourComponent::SetParkourType(EParkourType Type)
{
	if (!Character->HasAuthority())
	{
		Server_SetParkourType(Type);
	}
	
	if (ParkourType == Type)
	{
		return false;
	}
	PrevParkourType = ParkourType;
	//점프상태인지, 런상태인지 탐지하고
	//
	ParkourType = Type;
	ResetMovement();
	ParkourUpdate();
	return true;
}



void UParkourComponent::SlideEvent()
{
	if (ParkourType == EParkourType::EPT_WALL_MANTLE ||
		ParkourType == EParkourType::EPT_WALL_VERTICALRUN ||
		ParkourType == EParkourType::EPT_WALL_WALLGRAB)
	{
		VerticalWallRunEnd(0.5f);
	}
	if (IsWallRunning())
	{
		WallRunEnd(0.5f);
	}

	if (Character && Character->GetCharacterMovement()->IsWalking() &&
		CanSlide())
	{
		SlideStart();
	}
	else
	{

	}
}

void UParkourComponent::SprintEvent()
{
	SprintStart();
}

void UParkourComponent::ResetMovement()
{
	if (ParkourType == EParkourType::EPT_NONE)
	{
		Character->GetCharacterMovement()->GravityScale = DefaultGravity;
		Character->GetCharacterMovement()->GroundFriction = BaseGroundFriction;
		Character->GetCharacterMovement()->BrakingDecelerationWalking = BaseBrakingDecelerationWalking;
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseMaxWalkSpeed;
		Character->GetCharacterMovement()->MaxWalkSpeedCrouched = BaseMaxCrouchWalkSpeed;
		Character->GetCharacterMovement()->SetPlaneConstraintEnabled(false);
		switch (PrevParkourType)
		{
		case EParkourType::EPT_NONE:
			Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			break;

		case EParkourType::EPT_WALL_LEFTRUN:
			Character->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
			break;

		case EParkourType::EPT_WALL_RIGHTRUN:
			Character->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
			break;

		case EParkourType::EPT_WALL_VERTICALRUN:
			Character->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
			break;

		case EParkourType::EPT_WALL_WALLGRAB:
			Character->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
			break;

		case EParkourType::EPT_WALL_MANTLE:
			Character->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
			break;
		case EParkourType::EPT_WALL_SLIDE:
			Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			break;
		case EParkourType::EPT_WALL_SPRINT:
			Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			break;
		}
	}
}


void UParkourComponent::UpdateEvent()
{
	if (Character == nullptr) return;
	if (ZiplineEnable) return;

	if(WallUpdateDelay < 0.f)
		WallRunUpdate();

	if (VerticalWallRunDelay < 0.f && bVerticalWallRun)
	{
		VerticalWallRunUpdate();
	}

	if (MantleUpdateDelay < 0.f && bMantleUpdate)
	{
		MantleMovement();
	}
	if (SprintUpdateDelay < 0.f && bSprintUpdate)
	{
		SprintUpdate();
	}
	if (SLideUpdateDelay < 0.f && bSlideUpdate)
	{
		SlideUpdate();
	}
}

void UParkourComponent::OnRep_ParkourType()
{
	ResetMovement();
}

void UParkourComponent::WallRunUpdate()
{
	if (CanWallRun())
	{
		WallRunEnd(1.f);
		return;
	}
	if (ParkourType == EParkourType::EPT_WALL_MANTLE ||
		ParkourType == EParkourType::EPT_WALL_VERTICALRUN||
		ParkourType == EParkourType::EPT_WALL_WALLGRAB)
	{
		WallRunEnd(1.f);
		return;
	}
	if (Character)
	{
		FVector Start=Character->GetActorLocation();
		WallRunCheckVector();

		bool RightCheck = WallRunMovement(Start, Wall_RightVector, -1.f);
		bool LeftCheck = WallRunMovement(Start, Wall_LeftVector, 1.f);

		if (RightCheck || LeftCheck)
		{
			if (RightCheck)
			{
				if (SetParkourType(EParkourType::EPT_WALL_RIGHTRUN))
				{
					CorrectWallRunLocation();
				}
			}
			else if (LeftCheck)
			{
				if (SetParkourType(EParkourType::EPT_WALL_LEFTRUN))
				{
					CorrectWallRunLocation();
				}
			}

		}
		else
		{
			if (ParkourType == EParkourType::EPT_WALL_RIGHTRUN)
			{
				WallRunEnd(0.5f);
			}
			if (ParkourType == EParkourType::EPT_WALL_LEFTRUN)
			{
				WallRunEnd(0.5f);
			}
		}
	}
}

bool UParkourComponent::WallRunMovement(const FVector& Start, 
	const FVector& End, 
	float Direction)
{
	FHitResult HitResult; 
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(Character);
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, 
		ECollisionChannel::ECC_GameTraceChannel3, queryParams);

	if (HitResult.bBlockingHit)
	{
		if (Character && Character->GetMovementComponent()->IsFalling() && 
			VaildWallVector(HitResult.Normal) )
		{
			WallRunNormal= HitResult.Normal;
			WallRunLocation = HitResult.ImpactPoint;

			Character->LaunchCharacter(WallRunNormal.Cross(FVector(0.f, 0.f, 1.f)) * 
				WallRunSpeed*Direction, true, (!bWallRunGravity));
			return true;
		}
	}
	return false;
}

void UParkourComponent::WallRunEnd(float Delay)
{
	if (IsWallRunning())
	{
		SetParkourType(EParkourType::EPT_NONE);
		Character->GetCharacterMovement()->GravityScale = DefaultGravity;
		WallUpdateDelay = Delay;
	}
}

void UParkourComponent::CorrectWallRunLocation()
{
	if (!IsWallRunning()) return;
	FVector WallRunTargetVector;
	FRotator WallRunTargetRotator;

	WallRunTargetVector =WallRunLocation + 
		Character->GetCapsuleComponent()->GetUnscaledCapsuleRadius() * WallRunNormal;
	FRotator CapsuleRot=Character->GetCapsuleComponent()->GetRelativeRotation();
	float Yaw = 0.f;

	if (ParkourType == EParkourType::EPT_WALL_LEFTRUN)
		Yaw = 90.f;
	else
		Yaw = -90.f;
	
	FRotator Rot = WallRunNormal.Rotation();
	Yaw = Rot.Yaw - Yaw;
	WallRunTargetRotator = FRotator();
	WallRunTargetRotator.Yaw = Yaw;
	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	UKismetSystemLibrary::MoveComponentTo(
		Character->GetCapsuleComponent(),
		WallRunTargetVector, 
		WallRunTargetRotator,
		false, 
		false, 
		0.1f, 
		false,
		EMoveComponentAction::Move, 
		LatentInfo);
}

void UParkourComponent::CameraTick()
{
	switch (ParkourType)
	{
	case EParkourType::EPT_NONE:
		CameraTilt(0.f);
		break;
	case EParkourType::EPT_WALL_LEFTRUN:
		CameraTilt(15.f);
		break;
	case EParkourType::EPT_WALL_RIGHTRUN:
		CameraTilt(-15.f);
		break;
	case EParkourType::EPT_WALL_VERTICALRUN:
		CameraTilt(0.f);
		break;
	case EParkourType::EPT_WALL_WALLGRAB:
		CameraTilt(0.f);
		break;
	case EParkourType::EPT_WALL_MANTLE:
		CameraTilt(0.f);
		break;
	case EParkourType::EPT_WALL_SLIDE:
		CameraTilt(0.f);
		break;
	case EParkourType::EPT_WALL_SPRINT:
		CameraTilt(0.f);
		break;
	case EParkourType::EPT_END:
		CameraTilt(0.f);
		break;

	}
}

void UParkourComponent::CameraTilt(float Roll)
{
	if (Character)
	{
		FRotator CameraRotation = Character->GetController()->GetControlRotation();
		AGameStateBase* State = GetWorld()->GetGameState();
		UKismetMathLibrary::RInterpTo(CameraRotation, FRotator(Roll, CameraRotation.Pitch, CameraRotation.Yaw), State->GetServerWorldTimeSeconds(), 10.f);
		Character->GetController()->SetControlRotation(CameraRotation);
	}
}

void UParkourComponent::WallRunJump()
{
	if (IsWallRunning())
	{
		WallRunEnd(0.35f);
		if (Character)
		{
			FVector WallRunForce= WallRunNormal* WallRunJumpOffForce;
			WallRunForce.Z = WallRunJumpHeight;
			Character->LaunchCharacter(WallRunForce, false, true);
		}
	}
}

void UParkourComponent::WallRunCheckVector()
{
	if (Character)
	{
		FVector Location=Character->GetActorLocation();
		FVector RightVector = Character->GetActorRightVector();
		FVector ForwardVector = Character->GetActorForwardVector();
		
		FVector Right = RightVector * 75.f;
		FVector Back  = ForwardVector * -35.f;
		FVector Left = RightVector * -75.f;
		Wall_RightVector = Location + Right + Back;
		Wall_LeftVector = Location + Left + Back;
	}
}

void UParkourComponent::VerticalWallRunUpdate()
{
	MantleVector();
	if (CanVerticalWallRunning())
	{
		FHitResult HitResult;
		FCollisionQueryParams queryParams;
		queryParams.AddIgnoredActor(Character);
		TArray<AActor*> ActorToIgnore;
		ActorToIgnore.Add(Character);

		if (UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(),
			MantleEyes,
			MantleFeet,
			20.f,
			10.f,
			ETraceTypeQuery::TraceTypeQuery3,
			false,
			ActorToIgnore,
			EDrawDebugTrace::None,
			HitResult,
			true))
		{
			MantleTraceDistance = HitResult.Distance;
			LedgeFloorPosition = HitResult.ImpactPoint;
			FVector MantlePos = HitResult.ImpactPoint;
			MantlePos.Z += Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
			MantlePosition = MantlePos;
			{

				FHitResult HitResult2; 
				if (Character->GetCharacterMovement()->IsWalkable(HitResult) && ForwardTracer(HitResult2))
				{
					LedgeClimbWallNormal = HitResult2.ImpactNormal;
					Character->GetCharacterMovement()->DisableMovement();
					Character->GetCharacterMovement()->StopMovementImmediately();
					Character->GetCharacterMovement()->GravityScale = 0.f;
					bVerticalWallRun = false;
					if (SetParkourType(EParkourType::EPT_WALL_WALLGRAB))
					{
						bMantleUpdate = true;
					}
				}
				else
				{
					VerticalWallRunMovement();
				}

			}		
		}
		else
		{
			VerticalWallRunEnd(0.35f);
		}
	}
	else
	{
		VerticalWallRunEnd(0.35f);
	}
}

bool UParkourComponent::CanVerticalWallRunning()
{
	bool ret = Character && Character->GetCharacterMovement() && 
		Character->GetCharacterMovement()->IsFalling() &&
		ForwardInput() > 0.f && 
		(ParkourType == EParkourType::EPT_NONE || 
			ParkourType == EParkourType::EPT_WALL_VERTICALRUN || 
			IsWallRunning());
	return ret;
}

bool UParkourComponent::ForwardTracer(FHitResult& Result)
{
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(Character);
	FVector End = MantleFeet + (Character->GetActorForwardVector() * 30.f);
	TArray<AActor*> actor;
	return UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		MantleFeet,
		End,
		20.f,
		ETraceTypeQuery::TraceTypeQuery3,
		false,
		actor,
		EDrawDebugTrace::None,
		Result,
		true);

}

float UParkourComponent::ForwardInput()
{
	if (Character && Character->GetCharacterMovement())
	{
		FVector Forwardvec = Character->GetActorForwardVector();
		FVector LastInputVec = Character->GetCharacterMovement()->GetLastInputVector();
		return (float)Forwardvec.Dot(LastInputVec);
	}
	return 0.f;
}

void UParkourComponent::VerticalWallRunEnd(float Delay)
{
	if (ParkourType == EParkourType::EPT_WALL_VERTICALRUN ||
		ParkourType == EParkourType::EPT_WALL_MANTLE ||
		ParkourType == EParkourType::EPT_WALL_WALLGRAB)
	{
		VerticalWallRunDelay = Delay;
		bVerticalWallRun = true;
		bMantleUpdate = false;
		SetParkourType(EParkourType::EPT_NONE);
	}
}

void UParkourComponent::VerticalWallRunMovement()
{
	FHitResult HitResult;

	ForwardTracer(HitResult);
	if (ForwardInput() > 0.f && HitResult.bBlockingHit)
	{
		VerticalWallRunNormal = HitResult.ImpactNormal;
		if (SetParkourType(EParkourType::EPT_WALL_VERTICALRUN))
		{}
		else
		{
			FVector Normal = VerticalWallRunNormal * -1000.f;
			Character->LaunchCharacter(
				FVector(Normal.X, Normal.Y, VerticalWallRunSpeed), 
				true, true);
		}
	}
	else
	{
		VerticalWallRunEnd(0.35f);
		bMantleUpdate = false;
	}
}

bool UParkourComponent::VaildWallVector(const FVector& Vec)
{
	return UKismetMathLibrary::InRange_FloatFloat(Vec.Z, -0.52, 0.52);
}

void UParkourComponent::MantleVector()
{
	if (Character)
	{
		if (Character->GetController())
		{
			FVector OutLocation;
			FRotator OutRotation;
			Character->GetController()->GetActorEyesViewPoint(OutLocation, OutRotation);
			OutLocation.Z -= 40.f;
			FVector ForwardVec = Character->GetActorForwardVector();
			ForwardVec *= 30.f;
			MantleEyes = OutLocation + ForwardVec;

			FVector Location = Character->GetActorLocation();
			float CapsuleHeight=Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
			CapsuleHeight -= MantleHeight;
			Location.Z -= CapsuleHeight;
			MantleFeet = Location + ForwardVec;
		}
	}
}

bool UParkourComponent::QuickMantle()
{
	return MantleTraceDistance > Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
}

bool UParkourComponent::CanMantle()
{
	return ForwardInput() > 0.f &&
		(ParkourType == EParkourType::EPT_WALL_WALLGRAB || QuickMantle());

}

void UParkourComponent::LedgeGrabJump()
{
	if (ParkourType == EParkourType::EPT_WALL_WALLGRAB ||
		ParkourType == EParkourType::EPT_WALL_VERTICALRUN ||
		ParkourType == EParkourType::EPT_WALL_MANTLE )
	{
		VerticalWallRunEnd(0.35f);
		FVector WallOut= VerticalWallRunNormal*WallRunJumpOffForce;
		WallOut.Z = WallRunJumpHeight;
		Character->LaunchCharacter(WallOut, false, true);
	}
}

void UParkourComponent::SprintStart()
{
	SlideEnd();
	if (ParkourType == EParkourType::EPT_NONE && Character->GetCharacterMovement()->IsWalking())
	{
		SetParkourType(EParkourType::EPT_WALL_SPRINT);
		Character->GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		bSprintUpdate = true;
	}
}

void UParkourComponent::SprintUpdate()
{
	if (ParkourType == EParkourType::EPT_WALL_SPRINT)
	{
		if (ForwardInput())
		{

		}
		else
		{
			SprintEnd(0.15f);
		}
	}
}

void UParkourComponent::SprintEnd(float Delay)
{
	SetParkourType(EParkourType::EPT_NONE);
	SprintUpdateDelay = Delay;
	bSprintUpdate = false;
}

void UParkourComponent::SprintJump()
{
	if (ParkourType == EParkourType::EPT_WALL_SPRINT)
	{
		SprintEnd(0.f);
	}
}

void UParkourComponent::SlideStart()
{
	SprintEnd(0.f);
	SetParkourType(EParkourType::EPT_WALL_SLIDE);
	FVector Velocity = Character->GetCharacterMovement()->Velocity;
	Velocity.Normalize();
	FVector Location = Character->GetActorLocation();
	FVector UpVector = Character->GetActorUpVector();

	Character->GetCharacterMovement()->GroundFriction = 0.f;
	Character->GetCharacterMovement()->BrakingDecelerationWalking=1400.f;
	Character->GetCharacterMovement()->MaxWalkSpeed = 0.f;
	Character->GetCharacterMovement()->MaxWalkSpeedCrouched = 0.f;
	Character->GetCharacterMovement()->
		SetPlaneConstraintFromVectors(Velocity, UpVector);
	Character->GetCharacterMovement()->SetPlaneConstraintEnabled(true);

	FHitResult HitResult;
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(Character);

	GetWorld()->LineTraceSingleByChannel(HitResult,
		Location,
		Location + (UpVector * -200.f),
		ECollisionChannel::ECC_GameTraceChannel3,
		queryParams);
	//-1 슬라이드 반대방향나타나서
	FVector Cross = HitResult.ImpactNormal.Cross(Character->GetActorRightVector())*-1.f;

	//값이 높아지면 평평하지않음 앞으로 레이쏜거임
	if (Cross.Z < 0.02f)
	{
		Character->GetCharacterMovement()->AddImpulse(Cross * SlideImpulseAmount, true);
	}

	bSlideUpdate = true;
}

void UParkourComponent::SlideUpdate()
{
	if (ParkourType == EParkourType::EPT_WALL_SLIDE)
	{
		if (Character->GetCharacterMovement()->Velocity.Length() <= 35.f)
		{
			SlideEnd();
		}
	}
}

void UParkourComponent::SlideEnd()
{
	SetParkourType(EParkourType::EPT_NONE);
	bSlideUpdate = false;
	//Character->UnCrouch();
}

void UParkourComponent::SlideJump()
{
	if (ParkourType == EParkourType::EPT_WALL_SLIDE)
	{
		SlideEnd();
	}
}

void UParkourComponent::MantleMovement()
{
	FVector Location = Character->GetActorLocation();
	FRotator CurrentRot=Character->GetController()->GetControlRotation();

	FRotator LookatRotation=UKismetMathLibrary::FindLookAtRotation(FVector(Location.X, Location.Y, 0.f), FVector(MantlePosition.X, MantlePosition.Y, 0.f));
	FRotator interp=UKismetMathLibrary::RInterpTo(CurrentRot, LookatRotation, GetWorld()->DeltaTimeSeconds, 7.f);

	Character->GetController()->SetControlRotation(interp);

	float Speed = 7.f;
	if (QuickMantle())
	{
		Speed = 20.f;
	}
	else
	{
		Speed = 10.f;
	}
	FVector vecinterp = UKismetMathLibrary::VInterpTo(Location, MantlePosition, GetWorld()->DeltaTimeSeconds, Speed);
	Character->SetActorLocation(vecinterp);

	if (FVector::Distance(vecinterp, MantlePosition) < 3.f)
	{
		VerticalWallRunEnd(0.5f);
	}
}

bool UParkourComponent::IsWallRunning()
{
	return ParkourType == EParkourType::EPT_WALL_LEFTRUN || ParkourType == EParkourType::EPT_WALL_RIGHTRUN;
}

bool UParkourComponent::CanWallRun()
{
	return ForwardInput() > 0.f && (ParkourType == EParkourType::EPT_NONE || IsWallRunning());
}

bool UParkourComponent::CanSlide()
{
	return ForwardInput() > 0.f && (ParkourType == EParkourType::EPT_WALL_SPRINT || IsWallRunning());
}


void UParkourComponent::ZiplinePlay()
{
	FLatentActionInfo Info;
	Info.CallbackTarget = this;
	Info.UUID = 0;
	Info.ExecutionFunction = FName("ZiplineFinish");
	Info.Linkage = 0;
	FVector End = ZiplinePlayEnd - (FVector(0.f, 0.f, 40.f));
	float Time = (End - ZiplinePlayStart).Length() / ZiplineSpeed;

	UKismetSystemLibrary::MoveComponentTo(
		Character->GetCapsuleComponent(),
		End,
		Character->GetActorRotation(),
		false,
		true,
		Time,
		false,
		EMoveComponentAction::Type::Move,
		Info);
}

void UParkourComponent::ZiplineFinish()
{
	FRotator Rot=Character->GetActorRotation();
	Rot.Pitch = 0.f;
	Character->SetActorRotation(Rot);
	Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
	SetParkourType(EParkourType::EPT_NONE);
}

void UParkourComponent::SetZipline(bool Enable, const FVector& Start, const FVector& End)
{
	ZiplineEnable = Enable;
	ZiplineStart = Start;
	ZiplineEnd = End;
}

void UParkourComponent::Server_PlayerController_Implementation(AController* Controllers)
{
	AController* thisController=Character->GetController();
	thisController->SetControlRotation(Controllers->GetControlRotation());
}

void UParkourComponent::Server_SetParkourType_Implementation(EParkourType Type)
{
	if (ParkourType == Type)
	{
		return;
	}
	PrevParkourType = ParkourType;
	//점프상태인지, 런상태인지 탐지하고
	//
	ParkourType = Type;
}
