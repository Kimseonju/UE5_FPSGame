// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UE5_MultiPlayFPS/Performance/ObjectPoolActor.h"
#include "SkillDecal.generated.h"

UCLASS()
class UE5_MULTIPLAYFPS_API ASkillDecal : public AObjectPoolActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASkillDecal();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void ObjectPoolInUse(bool Enable);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:

	UPROPERTY(VisibleAnywhere,  meta = (AllowPrivateAccess = "true"))
		class USceneComponent* RootComp;
	UPROPERTY(VisibleAnywhere,  meta = (AllowPrivateAccess = "true"))
		class UDecalComponent* Circum;
	UPROPERTY(VisibleAnywhere,  meta = (AllowPrivateAccess = "true"))
		class UDecalComponent* Area;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UMaterialInstanceDynamic* CircumInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UMaterialInstanceDynamic* AreaInstance;

	float LifeTimeEnd;
	float LifeTime;

	UPROPERTY(EditAnywhere,  meta = (AllowPrivateAccess = "true"))
		bool bCircularSector;
	float Radius;

	float Damage;
	class UParticleSystem* ImpactParticle;

	class USoundCue* ImpactSound;

private:
	void Reset();
public:
	float GetLifeTimeEnd() { return LifeTimeEnd; }

	void PostInit(class UParticleSystem* _ImpactParticle, class USoundCue* _ImpactSound, float _Damage = 10.f, float _Radius = 0.f);
};
