// Fill out your copyright notice in the Description page of Project Settings.


#include "ShieldPickup.h"
#include "UE5_MultiPlayFPS/Character/MultiPlayCharacter.h"
#include "UE5_MultiPlayFPS/MultiPlayComponents/CombatComponent.h"
#include "UE5_MultiPlayFPS/MultiPlayComponents/BuffComponent.h"

AShieldPickup::AShieldPickup()
{
	ShieldAmount = 100.f;
	ShieldTime = 5.f;

}

void AShieldPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	AMultiPlayCharacter* PlayerCharacter = Cast<AMultiPlayCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		UBuffComponent* Buff = Cast<UBuffComponent>(PlayerCharacter->GetBuff());
		if (Buff)
		{
			Buff->ReplenishShield(ShieldAmount, ShieldTime);
		}
	}
	Destroy();
}
