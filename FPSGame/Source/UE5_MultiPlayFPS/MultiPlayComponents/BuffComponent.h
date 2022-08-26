#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE5_MULTIPLAYFPS_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()
	friend class AMultiPlayCharacter;

public:
	UBuffComponent();
	UFUNCTION(BlueprintCallable)
	void Heal(float HealAmount, float HealingTime);
	UFUNCTION(BlueprintCallable)
	void ReplenishShield(float ShieldAmount, float ReplenishTime);
protected:
	virtual void BeginPlay() override;
	void HealTick(float DeltaTime);
	void ReplenishShieldTick(float DeltaTime);
private:

	UPROPERTY(EditAnywhere, Category="BuffEffect", meta = (AllowPrivateAccess = "true"))
		class UParticleSystem* HealParticles;

	UPROPERTY(EditAnywhere, Category = "BuffEffect", meta = (AllowPrivateAccess = "true"))
		class UParticleSystem* ReplenishShieldParticles;

	class AMultiPlayCharacter* Character;
	bool bHealing;
	float HealingRate;
	float AmountToHeal;

	bool bReplenishShielding;
	float ReplenishShieldRate;
	float ReplenishAmountToShield;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
