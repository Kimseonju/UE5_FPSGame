
#include "NPC.h"
#include "Particles/ParticleSystemComponent.h"

ANPC::ANPC()
{
	PrimaryActorTick.bCanEverTick = false;
	ParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>("ParticleSystemComponent");
	ParticleComponent->SetupAttachment(GetMesh());
}

void ANPC::BeginPlay()
{
	Super::BeginPlay();
	
}

void ANPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANPC::QuestOn()
{
	ParticleComponent->SetVisibility(true);
}

void ANPC::QuestOff()
{
	ParticleComponent->SetVisibility(false);
}

void ANPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
