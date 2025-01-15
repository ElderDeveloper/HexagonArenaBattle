#include "HexGridManager.h"
#include "HexTile.h"


AHexGridManager::AHexGridManager()
{
	PrimaryActorTick.bCanEverTick = false;
	SetReplicates(true);
}


void AHexGridManager::GenerateHexGrid()
{
	if (!IsValid(HexTileClass))
	{
		UE_LOG(LogTemp, Error, TEXT("AHexGridManager HexTileClass is not valid"));
		return;
	}

	if (HexGridSet.Num() > 0)
	{
		DestroyHexGrid();
	}
	
	for (int q = -MapRadius; q <= MapRadius; q++)
	{
		const int32 R1 = std::max(-MapRadius, -q - MapRadius);
		const int32 R2 = std::min(MapRadius, -q + MapRadius);
		
		for (int r = R1; r <= R2; r++)
		{
			const float LocationX = -q * XOffset + r * HexWidth;
			const float LocationY =  q * HexHeight + YOffset*q;
			
			FVector Location = FVector(LocationX, LocationY, 10.f);
			
			AHexTile* HexTile = GetWorld()->SpawnActor<AHexTile>(HexTileClass, FTransform(FRotator::ZeroRotator,Location));
			HexTile->GridPosition = FIntPoint(q, r);
			HexTile->HexGridPosition = FHexGridPosition(q, r, -q-r);
			HexGridSet.Add(HexTile);
		}
	}
}


void AHexGridManager::DestroyHexGrid()
{
	for (AHexTile* HexTile : HexGridSet)
	{
		HexTile->Destroy();
	}
	HexGridSet.Empty();
}


float AHexGridManager::GetHeuristicCost(const AHexTile* TileA, const AHexTile* TileB) const
{
	FHexGridPosition PosA = TileA->HexGridPosition;
	FHexGridPosition PosB = TileB->HexGridPosition;
	
	return FMath::Max(
	FMath::Max(FMath::Abs(PosA.Q - PosB.Q), FMath::Abs(PosA.R - PosB.R)),
	FMath::Abs(PosA.S - PosB.S));
}


float AHexGridManager::GetDistanceCost(const AHexTile* TileA, const AHexTile* TileB) const
{
	return 1.0f;
}


AHexTile* AHexGridManager::GetTileWithLowestFScore(const TArray<AHexTile*>& OpenSet,const TMap<AHexTile*, float>& FScore) const
{
	AHexTile* LowestTile = OpenSet[0];
	float LowestScore = FScore[LowestTile];

	for (AHexTile* Tile : OpenSet)
	{
		if (FScore[Tile] < LowestScore)
		{
			LowestTile = Tile;
			LowestScore = FScore[Tile];
		}
	}

	return LowestTile;
}


TArray<AHexTile*> AHexGridManager::ReconstructPath(const TMap<AHexTile*, AHexTile*>& CameFrom,AHexTile* CurrentTile) const
{
	TArray<AHexTile*> Path;
	Path.Add(CurrentTile);
	while (CameFrom.Contains(CurrentTile))
	{
		CurrentTile = CameFrom[CurrentTile];
		Path.Insert(CurrentTile, 0);  // Insert at the beginning to maintain correct order
	}
	return Path;
}


void AHexGridManager::BeginPlay()
{
	Super::BeginPlay();
}


FVector AHexGridManager::GetRandomHexTileLocation(const float AddZPosition) const
{
	if (const auto& HexTile = GetRandomHexTile())
	{
		return HexTile->GetActorLocation() + FVector(0, 0, AddZPosition);
	}
	UE_LOG(LogTemp, Error, TEXT("Failed To Get GetRandomHexTileLocation"));
	return FVector::ZeroVector;
}


TArray<FVector> AHexGridManager::GetHexTileLocations(int32 Count) const
{
	if (HexGridSet.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("HexGridSet is empty"));
		return TArray<FVector>();
	}

	if (Count > HexGridSet.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("Count is greater than HexGridSet.Num()"));
		return TArray<FVector>();
	}
	
	TArray<FVector> HexTileLocations;
	TArray<int32> RandomIndexes;
	while (RandomIndexes.Num() < Count)
	{
		int32 RandomIndex = FMath::RandRange(0, HexGridSet.Num() - 1);
		if (!RandomIndexes.Contains(RandomIndex))
		{
			RandomIndexes.Add(RandomIndex);
		}
	}
	int32 CurrentIndex = 0;
	for (const auto& Item : HexGridSet)
	{
		if (RandomIndexes.Contains(CurrentIndex))
		{
			HexTileLocations.Add(Item->GetActorLocation());
		}
		CurrentIndex++;
	}

	return HexTileLocations;
}


AHexTile* AHexGridManager::GetRandomHexTile() const
{
	if (HexGridSet.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("HexGridSet is empty"));
		return nullptr;
	}
	
	int32 RandomSkipCount = FMath::RandRange(0, HexGridSet.Num() - 1);
	int32 CurrentIndex = 0;
	AHexTile* RandomItem = nullptr;

	for (const auto& Item : HexGridSet)
	{
		if (CurrentIndex == RandomSkipCount)
		{
			RandomItem = Item;
			break;
		}
		CurrentIndex++;
	}
	return RandomItem;
}


AHexTile* AHexGridManager::GetHexTileAt(const FHexGridPosition& HexGridPosition) const
{
	for (const auto& Item : HexGridSet)
	{
		if (Item->HexGridPosition == HexGridPosition)
		{
			return Item;
		}
	}
	return nullptr;
}


TArray<AHexTile*> AHexGridManager::GetNeighbourHexTiles(const FHexGridPosition& HexGridPosition) const
{
	TArray<AHexTile*> Neighbors;
	
	// Define 6 neighbor offsets in cube coordinates
	const TArray<FHexGridPosition> NeighborOffsets = {
		{+1, -1,  0},  // Right
		{+1,  0, -1},  // Top-Right
		{ 0, +1, -1},  // Top-Left
		{-1, +1,  0},  // Left
		{-1,  0, +1},  // Bottom-Left
		{ 0, -1, +1}   // Bottom-Right
	};
	
	for (const FHexGridPosition& Offset : NeighborOffsets)
	{
		FHexGridPosition NeighborPos;
		NeighborPos.Q = HexGridPosition.Q + Offset.Q;
		NeighborPos.R = HexGridPosition.R + Offset.R;
		NeighborPos.S = HexGridPosition.S + Offset.S;
		
		for (AHexTile* HexTile : HexGridSet)
		{
			if (HexTile->HexGridPosition == NeighborPos)
			{
				Neighbors.Add(HexTile);
				break;
			}
		}
	}
	return Neighbors;
}


TArray<AHexTile*> AHexGridManager::FindPathWithTiles(AHexTile* StartTile, AHexTile* EndTile) const
{
	TArray<AHexTile*> Path;
    if (!StartTile || !EndTile)
    {
        return Path;
    }

    // Open set (tiles to be evaluated) and closed set (tiles already evaluated)
    TSet<AHexTile*> OpenSet;
    TSet<AHexTile*> ClosedSet;

    // Maps to store G-costs and parent tiles for path reconstruction
    TMap<AHexTile*, float> GCost;
    TMap<AHexTile*, AHexTile*> ParentMap;

    // Initialize G-cost for the start tile
    GCost.Add(StartTile, 0.0f);

    // Add the start tile to the open set
    OpenSet.Add(StartTile);

    while (OpenSet.Num() > 0)
    {
        // Find the tile in the open set with the lowest F-cost
        AHexTile* CurrentTile = nullptr;
        float LowestFCost = TNumericLimits<float>::Max();
        for (AHexTile* Tile : OpenSet)
        {
            float CurrentGCost = GCost[Tile];
            float Heuristic = GetHeuristicCost(Tile, EndTile);
            float FCost = CurrentGCost + Heuristic;

            if (FCost < LowestFCost)
            {
                LowestFCost = FCost;
                CurrentTile = Tile;
            }
        }

        // If we reached the end tile, reconstruct the path
        if (CurrentTile == EndTile)
        {
        	AHexTile* Tile = EndTile;
            while (Tile)
            {
                Path.Insert(Tile, 0);  // Insert at the beginning to reverse the path
                Tile = ParentMap.Contains(Tile) ? ParentMap[Tile] : nullptr;
            }
            return Path;
        }

        // Move current tile from open to closed set
        OpenSet.Remove(CurrentTile);
        ClosedSet.Add(CurrentTile);

        // Check neighbors of the current tile
        TArray<AHexTile*> Neighbors = GetNeighbourHexTiles(CurrentTile->HexGridPosition);
        for (AHexTile* Neighbor : Neighbors)
        {
            if (ClosedSet.Contains(Neighbor))
            {
                continue;  // Skip already evaluated tiles
            }

            // Calculate tentative G-cost for the neighbor
            float TentativeGCost = GCost[CurrentTile] + GetDistanceCost(CurrentTile, Neighbor);

            if (!OpenSet.Contains(Neighbor))
            {
                OpenSet.Add(Neighbor);
            }
            else if (TentativeGCost >= GCost[Neighbor])
            {
                continue;  // Skip if not a better path
            }

            // Update G-cost and parent for the neighbor
            GCost.Add(Neighbor, TentativeGCost);
            ParentMap.Add(Neighbor, CurrentTile);
        }
    }

    // If we exit the loop without finding a path, return an empty array
    return Path;
}


TArray<AHexTile*> AHexGridManager::FindPathWithTilesWithAvoidance(AHexTile* StartTile, AHexTile* EndTile) const
{
	TArray<AHexTile*> OpenSet;
	TSet<AHexTile*> ClosedSet;
	TMap<AHexTile*, AHexTile*> CameFrom;
	TMap<AHexTile*, float> GScore;
	TMap<AHexTile*, float> FScore;

	OpenSet.Add(StartTile);
	GScore.Add(StartTile, 0.0f);
	FScore.Add(StartTile, GetHeuristicCost(StartTile, EndTile));

	while (OpenSet.Num() > 0)
	{
		// Find the tile in OpenSet with the lowest FScore
		AHexTile* CurrentTile = GetTileWithLowestFScore(OpenSet, FScore);

		if (CurrentTile == EndTile)
		{
			return ReconstructPath(CameFrom, CurrentTile);
		}

		OpenSet.Remove(CurrentTile);
		ClosedSet.Add(CurrentTile);

		// Iterate through neighbors
		for (AHexTile* Neighbor : GetNeighbourHexTiles(CurrentTile->HexGridPosition))
		{
			// Skip tiles that are already occupied by another actor
			if (Neighbor->GetOccupyingActor() != nullptr && Neighbor != EndTile)
			{
				continue;
			}

			if (ClosedSet.Contains(Neighbor))
			{
				continue;
			}

			float TentativeGScore = GScore[CurrentTile] + GetDistanceCost(CurrentTile,Neighbor);

			if (!OpenSet.Contains(Neighbor))
			{
				OpenSet.Add(Neighbor);
			}
			else if (TentativeGScore >= GScore[Neighbor])
			{
				continue;
			}

			// Record best path to the neighbor
			CameFrom.Add(Neighbor, CurrentTile);
			GScore.Add(Neighbor, TentativeGScore);
			FScore.Add(Neighbor, TentativeGScore + GetHeuristicCost(Neighbor, EndTile));
		}
	}

	// Return an empty path if no valid path found
	return TArray<AHexTile*>();
}
