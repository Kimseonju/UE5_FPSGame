#pragma once

#include "CoreMinimal.h"
#include "UE5_MultiPlayFPS/Performance/ObjectPoolActor.h"
#include "Projectile.generated.h"
class USoundCue;
UCLASS()
class UE5_MULTIPLAYFPS_API AProjectile : public AObjectPoolActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
	virtual void ObjectPoolInUse(bool Enable);
protected:
	virtual void BeginPlay() override;
	void StartDestroyTimer();
	void DestroyTimerFinished();
	void ExplodeDamage();
	void SpawnTrailSystem();
	UFUNCTION()

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere)
		float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float FieldDamage;
	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	USoundCue* ImpactSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UFieldSystemComponent* FieldSystemComponent;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* TrailSystem;

	UPROPERTY()
	class UNiagaraComponent* TrailSystemComponent;

	UPROPERTY(VisibleAnywhere)
		class UProjectileMovementComponent* ProjectileMovementComponent;
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(EditAnywhere)
	float DamageInnerRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DamageOuterRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float DestroyTime;
private:	

	UPROPERTY(EditAnywhere)
	class UParticleSystem* Tracer;

	UPROPERTY()
	class UParticleSystemComponent* TracerComponent;


	FTimerHandle DestroyTimer;

public:
	UFUNCTION(NetMulticast, Reliable)
		void MultiCastDestroyEffect(UParticleSystem* Particles, USoundCue* Sound);
	UFUNCTION(BlueprintCallable)
	virtual void SetVelocity(const FVector& Velocity, float Speed);
};
