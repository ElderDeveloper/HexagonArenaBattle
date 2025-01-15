#pragma once

#include "CoreMinimal.h"
#include "HexGridManager.h"
#include "GameFramework/Actor.h"
#include "HexTile.generated.h"

class UStaticMeshComponent;

UCLASS()
class ILLUVIUM_API AHexTile : public AActor
{
	GENERATED_BODY()

public:
	AHexTile();

	// Grid coordinates in 2D space for Rectangular Grid
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = "HexTile")
	FIntPoint GridPosition;

	// Grid coordinates in cube coordinates for Hexagonal Grid
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = "HexTile")
	FHexGridPosition HexGridPosition;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "HexTile")
	bool DebugHexTile = false;

	UFUNCTION(BlueprintCallable)
	bool SetOccupyingActor(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	bool ClearOccupyingActor();

	UFUNCTION(BlueprintPure)
	AActor* GetOccupyingActor() const { return OccupyingActor; }

	UFUNCTION(BlueprintPure)
	bool IsOccupied() const { return OccupyingActor != nullptr; }
	
protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "HexTile")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(Replicated)
	AActor* OccupyingActor;

	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};

