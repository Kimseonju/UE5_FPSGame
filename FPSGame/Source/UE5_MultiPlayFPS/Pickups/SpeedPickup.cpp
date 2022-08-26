// Fill out your copyright notice in the Description page of Project Settings.


#include "SpeedPickup.h"
#include "UE5_MultiPlayFPS/Character/MultiPlayCharacter.h"
#include "UE5_MultiPlayFPS/MultiPlayComponents/BuffComponent.h"

ASpeedPickup::ASpeedPickup()
{
	SpeedBuff = 1600.f;
	CrouchSpeedBuff = 850.f;
	SpeedBuffTime = 30.f;
}

void ASpeedPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	//AMultiPlayCharacter* PlayerCharacter = Cast<AMultiPlayCharacter>(OtherActor);
	//if (PlayerCharacter)
	//{
	//	UBuffComponent* Buff = Cast<UBuffComponent>(PlayerCharacter->GetBuff());
	//	if (Buff)
	//	{
	//		Buff->BuffSpeed(SpeedBuff, CrouchSpeedBuff, SpeedBuffTime);
	//	}
	//}
	//Destroy();
}
