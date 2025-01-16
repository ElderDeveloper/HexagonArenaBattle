#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HexGridManager.generated.h"

class AHexTile;

USTRUCT(BlueprintType)
struct FHexGridPosition
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	int32 Q;
	UPROPERTY(BlueprintReadWrite)
	int32 R;
	UPROPERTY(BlueprintReadWrite)
	int32 S;

	FHexGridPosition() : Q(0), R(0), S(0) {}
	FHexGridPosition(int32 q, int32 r, int32 s) : Q(q), R(r), S(s) {}

	bool operator==(const FHexGridPosition& Position) const
	{
		return Q == Position.Q && R == Position.R && S == Position.S;
	}
	
};





UCLASS()
class ILLUVIUM_API AHexGridManager : public AActor
{
	GENERATED_BODY()

public:
	AHexGridManager();
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "HexGrid|Tile")
	TSubclassOf<AHexTile> HexTileClass;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "HexGrid|Tile")
	int32 MapRadius = 2;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "HexGrid|Tile")
	float XOffset = -42.f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "HexGrid|Tile")
	float YOffset = -25.f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "HexGrid|Tile")
	float HexHeight = 100.f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "HexGrid|Tile")
	float HexWidth = 85.f;

	UFUNCTION(BlueprintCallable, Category="HexGrid")
	void GenerateHexGrid();

	UFUNCTION(BlueprintCallable, Category="HexGrid")
	void DestroyHexGrid();

protected:
	TSet<AHexTile*> HexGridSet;
	
	float GetHeuristicCost(const AHexTile* TileA, const AHexTile* TileB) const;
	float GetDistanceCost(const AHexTile* TileA, const AHexTile* TileB) const;
	AHexTile* GetTileWithLowestFScore(const TArray<AHexTile*>& OpenSet, const TMap<AHexTile*, float>& FScore) const;
	TArray<AHexTile*> ReconstructPath(const TMap<AHexTile*, AHexTile*>& CameFrom, AHexTile* CurrentTile) const;
	
	virtual void BeginPlay() override;

public:
	
	UFUNCTION(BlueprintPure, Category="HexGrid")
	FVector GetRandomHexTileLocation(const float AddZPosition = 0) const;

	UFUNCTION(BlueprintPure, Category="HexGrid")
	TArray<FVector> GetHexTileLocations(int32 Count = 4) const;

	UFUNCTION(BlueprintPure, Category="HexGrid")
	AHexTile* GetRandomHexTile() const;

	UFUNCTION(BlueprintPure, Category="HexGrid")
	AHexTile* GetHexTileAt(const FHexGridPosition& HexGridPosition) const;

	UFUNCTION(BlueprintPure, Category="HexGrid")
	TArray<AHexTile*> GetNeighbourHexTiles(const FHexGridPosition& HexGridPosition) const;

	UFUNCTION(BlueprintCallable, Category="HexGrid")
	TArray<AHexTile*> FindPathWithTiles(AHexTile* StartTile,AHexTile* EndTile) const;

	UFUNCTION(BlueprintCallable, Category="HexGrid")
	TArray<AHexTile*> FindPathWithTilesWithAvoidance(AHexTile* StartTile,AHexTile* EndTile) const;
};

