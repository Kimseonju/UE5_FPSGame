// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpPickup.h"
#include "UE5_MultiPlayFPS/Character/MultiPlayCharacter.h"
#include "UE5_MultiPlayFPS/MultiPlayComponents/BuffComponent.h"

AJumpPickup::AJumpPickup()
{
	JumpVelocity = 4000.f;
	JumpBuffTime = 30.f;
}

void AJumpPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	//AMultiPlayCharacter* PlayerCharacter = Cast<AMultiPlayCharacter>(OtherActor);
	//if (PlayerCharacter)
	//{
	//	UBuffComponent* Buff = Cast<UBuffComponent>(PlayerCharacter->GetBuff());
	//	if (Buff)
	//	{
	//		Buff->BuffJump(JumpVelocity, JumpBuffTime);
	//	}
	//}
	//Destroy();
}
