// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPickup.h"
#include "UE5_MultiPlayFPS/Character/MultiPlayCharacter.h"
#include "UE5_MultiPlayFPS/MultiPlayComponents/CombatComponent.h"
#include "UE5_MultiPlayFPS/MultiPlayComponents/BuffComponent.h"


AHealthPickup::AHealthPickup()
{
	HealAmount = 100.f;
	HealingTime = 5.f;

}

void AHealthPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	AMultiPlayCharacter* PlayerCharacter = Cast<AMultiPlayCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		UBuffComponent* Buff = Cast<UBuffComponent>(PlayerCharacter->GetBuff());
		if (Buff)
		{
			Buff->Heal(HealAmount, HealingTime);
		}
	}
	Destroy();
}
