#include "BuffComponent.h"
#include "UE5_MultiPlayFPS/Character/MultiPlayCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
UBuffComponent::UBuffComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bHealing=false;
	HealingRate=0.f;
	AmountToHeal = 0.f;
	bReplenishShielding = false;
	ReplenishShieldRate = 0.f;
	ReplenishAmountToShield = 0.f;
}

void UBuffComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBuffComponent::Heal(float HealAmount, float HealingTime)
{
	bHealing = true;
	HealingRate = HealAmount / HealingTime;
	AmountToHeal = HealAmount;
}
void UBuffComponent::HealTick(float DeltaTime)
{
	if (!bHealing || Character == nullptr || Character->IsElimmed()) return;
	const float HealThisFrame = HealingRate * DeltaTime;
	Character->SetHealth(
		FMath::Clamp(Character->GetHealth() + HealThisFrame, 0.f, Character->GetMaxHealth()));

	Character->UpdateHUDHealth();
	AmountToHeal -= HealThisFrame;
	if (AmountToHeal <= 0.f || Character->GetHealth() >= Character->GetMaxHealth())
	{
		bHealing = false;
		AmountToHeal = 0.f;
	}
}

void UBuffComponent::ReplenishShield(float ShieldAmount, float ReplenishTime)
{
	bReplenishShielding = true;
	ReplenishShieldRate = ShieldAmount / ReplenishTime;
	ReplenishAmountToShield = ShieldAmount;
}

void UBuffComponent::ReplenishShieldTick(float DeltaTime)
{
	if (!bReplenishShielding || Character == nullptr || Character->IsElimmed()) return;

	const float ShieldThisFrame = ReplenishShieldRate * DeltaTime;
	Character->SetShield(FMath::Clamp(Character->GetShield() + ShieldThisFrame, 0.f, Character->GetMaxShield()));
	Character->UpdateHUDShield();
	ReplenishAmountToShield -= ShieldThisFrame;

	if (ReplenishAmountToShield <= 0.f || Character->GetShield() >= Character->GetMaxShield())
	{
		bReplenishShielding = false;
		ReplenishAmountToShield = 0.f;
	}
}

void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	HealTick(DeltaTime);
	ReplenishShieldTick(DeltaTime);
}

