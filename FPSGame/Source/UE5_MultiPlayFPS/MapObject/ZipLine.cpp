#include "ZipLine.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UE5_MultiPlayFPS/Interface/ZiplineInterface.h"
#include "CableComponent.h"
AZipLine::AZipLine()
{
	PrimaryActorTick.bCanEverTick = true;
	Rootcom = CreateDefaultSubobject<USceneComponent>("Rootcom");
	Rootcom->SetupAttachment(RootComponent);

	StartMesh = CreateDefaultSubobject<UStaticMeshComponent>("StartMesh");
	StartMesh->SetupAttachment(Rootcom);
	StartTop = CreateDefaultSubobject<USceneComponent>("StartTop");
	StartTop->SetupAttachment(StartMesh);
	StartCollision = CreateDefaultSubobject<UBoxComponent>("StartCollision");
	StartCollision->SetupAttachment(StartMesh);

	EndMesh = CreateDefaultSubobject<UStaticMeshComponent>("EndMesh");
	EndMesh->SetupAttachment(Rootcom);
	EndTop = CreateDefaultSubobject<USceneComponent>("EndTop");
	EndTop->SetupAttachment(EndMesh);
	EndCollision = CreateDefaultSubobject<UBoxComponent>("EndCollision");
	EndCollision->SetupAttachment(EndMesh);

	Cable = CreateDefaultSubobject<UCableComponent>("Cable");
	Cable->SetupAttachment(Rootcom);
}

void AZipLine::BeginPlay()
{
	Super::BeginPlay();

	Cable->SetWorldLocation(StartTop->GetComponentLocation());
	Cable->SetAttachEndToComponent(EndTop);
	StartCollision->OnComponentBeginOverlap.AddDynamic(this, &AZipLine::StartTopCollisionBeginOverlap);
	StartCollision->OnComponentEndOverlap.AddDynamic(this, &AZipLine::StartTopCollisionEndOverlap);

	EndCollision->OnComponentBeginOverlap.AddDynamic(this, &AZipLine::EndTopCollisionBeginOverlap);
	EndCollision->OnComponentEndOverlap.AddDynamic(this, &AZipLine::EndTopCollisionEndOverlap);
}

void AZipLine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AZipLine::StartTopCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IZiplineInterface* OtherActorInterface = Cast<IZiplineInterface>(OtherActor);
	if (OtherActorInterface)
	{
		OtherActorInterface->SetZipline(true, StartTop->GetComponentLocation(), EndTop->GetComponentLocation());
	}
}

void AZipLine::StartTopCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	IZiplineInterface* OtherActorInterface = Cast<IZiplineInterface>(OtherActor);
	if (OtherActorInterface)
	{
		OtherActorInterface->SetZipline(false, FVector(), FVector());
	}
}

void AZipLine::EndTopCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IZiplineInterface* OtherActorInterface = Cast<IZiplineInterface>(OtherActor);
	if (OtherActorInterface)
	{
		OtherActorInterface->SetZipline(true, EndTop->GetComponentLocation(), StartTop->GetComponentLocation());
	}
}

void AZipLine::EndTopCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	IZiplineInterface* OtherActorInterface = Cast<IZiplineInterface>(OtherActor);
	if (OtherActorInterface)
	{
		OtherActorInterface->SetZipline(false, FVector(), FVector());
	}
}

