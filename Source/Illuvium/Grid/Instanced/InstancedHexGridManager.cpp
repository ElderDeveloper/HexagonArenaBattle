
#include "InstancedHexGridManager.h"
#include "Components/InstancedStaticMeshComponent.h"


// Sets default values
AInstancedHexGridManager::AInstancedHexGridManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	HexMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("HexMesh"));
	RootComponent = HexMesh;
	HexSize = 100.f;
}

void AInstancedHexGridManager::GenerateRectangularGrid()
{
	for (int32 y = 0; y < GridHeight; y++)
	{
		for (int32 x = 0; x < GridWidth; x++)
		{
			float XOffset = (x + 0.5f * (y % 2)) * FMath::Sqrt(3.f) * HexSize ;
			float YOffset = y * HexSize * 1.5f;

			FVector HexLocation(XOffset, YOffset, 0.0f);
			FTransform HexTransform(FRotator::ZeroRotator, HexLocation, FVector(1.0f));

			HexMesh->AddInstance(HexTransform);
		}
	}
}

// Called when the game starts or when spawned
void AInstancedHexGridManager::BeginPlay()
{
	Super::BeginPlay();
	GenerateRectangularGrid();
}

