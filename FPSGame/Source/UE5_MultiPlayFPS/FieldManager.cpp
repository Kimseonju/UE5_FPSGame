#include "FieldManager.h"
#include "Character/Enemy.h"
#include "Kismet/GameplayStatics.h"
AFieldManager::AFieldManager()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AFieldManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFieldManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

TArray<AActor*> AFieldManager::GetAllEnemy()
{
	TArray<AActor*> AllEnemy;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), AllEnemy);
	return AllEnemy;
}

