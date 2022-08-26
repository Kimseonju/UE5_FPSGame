// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ParkourComponent.generated.h"

UENUM(BlueprintType)
enum class EParkourType :uint8
{
	EPT_NONE UMETA(DisplayName = "None"),
	EPT_WALL_LEFTRUN UMETA(DisplayName = "Wall_LeftRun"),
	EPT_WALL_RIGHTRUN UMETA(DisplayName = "Wall_RightRun"),
	EPT_WALL_VERTICALRUN UMETA(DisplayName = "Wall_Vertical"),
	EPT_WALL_WALLGRAB UMETA(DisplayName = "Grab"),
	EPT_WALL_MANTLE UMETA(DisplayName = "Mantle"),
	EPT_WALL_SLIDE UMETA(DisplayName = "SLide"),
	EPT_WALL_SPRINT UMETA(DisplayName = "Sprint"),
	EPT_ZIPLINE UMETA(DisplayName = "Zipline"),
	EPT_END UMETA(DisplayName = "END")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE5_MULTIPLAYFPS_API UParkourComponent : public UActorComponent
{
	GENERATED_BODY()
		friend class AMultiPlayCharacter;

public:	
	UParkourComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ParkourUpdate();
	void JumpEvent();
	void LandEvent();
	void ResetMovement();
	bool SetParkourType(EParkourType Type);


	
	void SlideEvent();
	void SprintEvent();
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	void UpdateEvent();
private:

	UPROPERTY()
		class AMultiPlayCharacter* Character;
	UPROPERTY()
		class AMultiPlayPlayerController* Controller;
	UPROPERTY()
		class AMultiPlayHUD* HUD;

	

	UPROPERTY(ReplicatedUsing=OnRep_ParkourType, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EParkourType ParkourType;
	UFUNCTION()
	void OnRep_ParkourType();
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EParkourType PrevParkourType;

	FTimerHandle WallRunTimer;

	float DefaultGravity;
	float WallRunTargetGravity;
	float WallRunNormalize;
	UPROPERTY(EditAnywhere)
	float WallRunSpeed;

	UPROPERTY(EditAnywhere)
	float WallRunJumpHeight;
	float WallRunJumpOffForce;

	bool bWallRunGravity;

	FVector Wall_RightVector;
	FVector Wall_LeftVector;
	FVector WallRunNormal;
	FVector WallRunLocation;
	float WallUpdateDelay;
	
	void WallRunUpdate();
	bool WallRunMovement(const FVector& Start, const FVector& End, float Direction);
	void WallRunEnd(float Delay);
	void CorrectWallRunLocation();
	void CameraTick();
	void CameraTilt(float Roll);
	void WallRunJump();

	void WallRunCheckVector();

	FVector MantleEyes;
	FVector MantleFeet;
	FVector MantlePosition;
	UPROPERTY(EditAnywhere)
	float MantleHeight;
	float MantleTraceDistance;
	float VerticalWallRunDelay;
	bool bVerticalWallRun;
	UPROPERTY(EditAnywhere)
	float VerticalWallRunSpeed;
	FVector LedgeFloorPosition;
	FVector VerticalWallRunNormal;
	FVector VerticalWallRunLocation;
	FVector LedgeClimbWallNormal;
	bool bMantleUpdate;
	float MantleUpdateDelay;
	
	void VerticalWallRunUpdate();
	bool CanVerticalWallRunning();
	bool ForwardTracer(FHitResult& Result);
	float ForwardInput();
	void VerticalWallRunEnd(float Delay);
	void VerticalWallRunMovement();
	bool VaildWallVector(const FVector& Vec);
	void MantleVector();
	
	void MantleMovement();

	bool QuickMantle();
	bool CanMantle();

	void LedgeGrabJump();
	//안써서 지우고싶지만 지우면 블루프린트먹통..
	UPROPERTY(EditAnywhere, Category = Combat)
		class UAnimMontage* VerticalWallRunEndMontage;
	UPROPERTY(EditAnywhere)
	float SprintSpeed;
	bool bSprintUpdate;
	float SprintUpdateDelay;
	void SprintStart();
	void SprintUpdate();
	void SprintEnd(float Delay);
	void SprintJump();

	bool bSlideUpdate;
	float SLideUpdateDelay;
	float BaseGroundFriction;
	float BaseBrakingDecelerationWalking;
	float BaseMaxWalkSpeed;
	float BaseMaxCrouchWalkSpeed;

	float GroundFriction;
	float BrakingDecelerationWalking;
	float SlideGroundFriction;
	float SlideBrakingDecelerationWalking;
	UPROPERTY(EditAnywhere)
	float SlideImpulseAmount;
	void SlideStart();
	void SlideUpdate();
	void SlideEnd();
	void SlideJump();

	bool IsWallRunning();
	bool CanWallRun();
	bool CanSlide();

	UPROPERTY(EditAnywhere)
	float ZiplineSpeed;

	bool ZiplineEnable;
	FVector ZiplineStart;
	FVector ZiplineEnd;
	FVector ZiplinePlayStart;
	FVector ZiplinePlayEnd;

	UFUNCTION(BlueprintCallable)
	void ZiplinePlay();
	UFUNCTION(BlueprintCallable)
	void ZiplineFinish();

	void SetZipline(bool Enable, const FVector& Start, const FVector& End);
	UFUNCTION(Server, Reliable)
		void Server_PlayerController(class AController* Controllers);

	UFUNCTION(Server, Reliable)
	void Server_SetParkourType(EParkourType Type);

	
public:
	UFUNCTION()
	EParkourType GetParkourType() { return ParkourType;}
};
