// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Illuvium/Grid/HexGridManager.h"
#include "GridCharacter.generated.h"

class AHexGridManager;
class AHexTile;


UENUM(BlueprintType)
enum class EGridCharacterTeam : uint8
{
	// Added None since if the Variable does not change, the OnRep functions does not get called 
	None,
	TeamBlue,
	TeamRed
};

UCLASS()
class ILLUVIUM_API AGridCharacter : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGridCharacter();

	UFUNCTION(BlueprintCallable)
	void AddPlayerToTeam(const EGridCharacterTeam& NewTeam) { Team = NewTeam; if(HasAuthority()) OnRep_Team(); }

	UFUNCTION(BlueprintCallable)
	void SetGridManager(AHexGridManager* GridManager) { HexGridManager = GridManager; }

	UFUNCTION(BlueprintCallable)
	void SetPath(const TArray<AHexTile*>& NewPath);

	UFUNCTION(BlueprintCallable)
	void SetClosestEnemy(AGridCharacter* Enemy) { ClosestEnemy = Enemy; }

	UFUNCTION(BlueprintCallable)
	void AssignTile(AHexTile* Tile);

protected:

	UPROPERTY(Replicated)
	uint8 MaxHealth = 10;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentHealth)
	uint8 CurrentHealth = MaxHealth;

	UPROPERTY(Replicated)
	uint8 LastTakenDamage = 1;

	UPROPERTY(ReplicatedUsing=OnRep_Team,BlueprintReadWrite,Category = "GridCharacter")
	EGridCharacterTeam Team;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "GridCharacter")
	FVector CharacterRelativePositionOnTile = FVector(0.f, 0.f, 100.f);

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "GridCharacter")
	float MoveSpeed = 5.f;
	
	UPROPERTY()
	AHexTile* CurrentHexTile;

	UPROPERTY()
	AHexGridManager* HexGridManager;

	UPROPERTY(Replicated)
	AGridCharacter* ClosestEnemy;

	UPROPERTY(ReplicatedUsing=OnRep_Path)
	TArray<AHexTile*> Path;

	FVector TargetGridLocation;
	FVector StartLocation;

	FTimerHandle PathMoveTimerHandle;
	float Alpha = 0.f;

	UFUNCTION()
	virtual void OnRep_Team();
	
	UFUNCTION()
	virtual void OnRep_Path();

	UFUNCTION()
	virtual void OnRep_CurrentHealth();

	FTimerHandle ApplyDamageTimerHandle;
	void ApplyDamageToClosestEnemy();
	void ReceiveDamage(uint8 DamageTaken);
	bool CheckNeighbourTilesForEnemy();

	void AssignCurrentHexTile(bool ClearCurrentTile = false);

	UFUNCTION(BlueprintImplementableEvent)
	void OnTeamChanged(const EGridCharacterTeam& NewTeam);

	UFUNCTION(BlueprintImplementableEvent)
	void OnApplyDamage(AGridCharacter* Enemy, uint8 Damage);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnTookDamage(uint8 Damage, uint8 RemainingHealth, uint8 MaximumHealth);
	
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:

	UFUNCTION(BlueprintPure)
	EGridCharacterTeam GetTeam() const { return Team; }

	UFUNCTION(BlueprintPure)
	AGridCharacter* GetClosestEnemy() const { return ClosestEnemy; }

	UFUNCTION(BlueprintPure)
	TArray<AHexTile*> GetPath() const { return Path; }

	UFUNCTION(BlueprintCallable)
	AHexTile* GetCurrentHexTile() const { return CurrentHexTile; }
};


