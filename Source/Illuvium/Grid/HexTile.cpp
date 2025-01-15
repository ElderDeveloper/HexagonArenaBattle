#include "HexTile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Net/UnrealNetwork.h"


AHexTile::AHexTile()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetGenerateOverlapEvents(false);
	MeshComponent->SetCastShadow(false);
	MeshComponent->SetCanEverAffectNavigation(false);
	MeshComponent->SetupAttachment(RootComponent);

	SetReplicates(true);
}


bool AHexTile::SetOccupyingActor(AActor* Actor)
{
	if (!OccupyingActor)
	{
		OccupyingActor = Actor;
		return true;
	}
	return false;
}


bool AHexTile::ClearOccupyingActor()
{
	if (OccupyingActor)
	{
		OccupyingActor = nullptr;
		return true;
	}
	return false;
}


void AHexTile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (DebugHexTile)
	{
		DrawDebugString( GetWorld(), GetActorLocation(), FString::Printf(TEXT("Q: %d, R: %d, S: %d"), HexGridPosition.Q, HexGridPosition.R, HexGridPosition.S), nullptr, FColor::White, DeltaSeconds);
	}
	
}


inline void AHexTile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AHexTile, OccupyingActor);
}
