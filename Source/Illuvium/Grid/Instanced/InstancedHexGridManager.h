#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InstancedHexGridManager.generated.h"

UCLASS()
class ILLUVIUM_API AInstancedHexGridManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AInstancedHexGridManager();

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "HexGrid")
	UInstancedStaticMeshComponent* HexMesh;

	float HexSize = 100.0f; // Size of each hexagon
	int32 GridWidth = 10;   // Number of hexagons along the width
	int32 GridHeight = 10; 

protected:

	void GenerateRectangularGrid();

	
	virtual void BeginPlay() override;

};
