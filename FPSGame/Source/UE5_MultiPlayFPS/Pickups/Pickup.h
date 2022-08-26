#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UE5_MultiPlayFPS/Interface/PickupItemInterface.h"
#include "Pickup.generated.h"

UCLASS()
class UE5_MULTIPLAYFPS_API APickup : public AActor, public IPickupItemInterface

{
	GENERATED_BODY()
	
public:	
	APickup();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);
private:


	UPROPERTY(EditAnywhere)
	class USphereComponent* OverlapSphere;

	UPROPERTY(EditAnywhere)
	class USoundCue* PickupSound;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* PickupMesh;

	UPROPERTY(VisibleAnywhere)
		class UNiagaraComponent* PickupEffectComponent;

	UPROPERTY(EditAnywhere)
		class UNiagaraSystem* PickupEffect;

	UFUNCTION(NetMulticast, Reliable)
		void MultiCastDestroyEffect();


	FTimerHandle BindOverlapTimer;
	float BindOverlapTime;
	void BindOverlapFinish();
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FItemInfo ItemInfo;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UWidgetComponent* PickupWidget;
public:
	virtual void ShowPickupWidget(bool bShowWidget) override;

};
