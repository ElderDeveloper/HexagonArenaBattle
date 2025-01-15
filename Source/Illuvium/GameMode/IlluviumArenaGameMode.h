// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "IlluviumArenaGameMode.generated.h"

class AGridCharacter;
class AHexGridManager;
/**
 * 
 */
UCLASS()
class ILLUVIUM_API AIlluviumArenaGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AIlluviumArenaGameMode();

	UFUNCTION(BlueprintCallable)
	void SpawnHexGridManager();

	UFUNCTION(BlueprintCallable)
	void SpawnGridCharacters();

protected:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AHexGridManager> HexGridManagerClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGridCharacter> GridCharacterClass;

	UPROPERTY(EditDefaultsOnly)
	uint8 GridCharacterCount = 10;

	UPROPERTY(ReplicatedUsing=OnRep_HexGridManager)
	AHexGridManager* HexGridManager;

	UFUNCTION()
	void OnRep_HexGridManager();

	UPROPERTY(Replicated)
	TArray<AGridCharacter*> GridCharacters;

	UPROPERTY(Replicated)
	TArray<AGridCharacter*> BlueTeamCharacters;

	UPROPERTY(Replicated)
	TArray<AGridCharacter*> RedTeamCharacters;

	UPROPERTY(Replicated)
	FIntPoint TeamCharacterCount;

	void AddCharacterToTeam(AGridCharacter* Character);

	UPROPERTY(EditDefaultsOnly , Category = "Path")
	float PathLoopInterval = 1.f;
	FTimerHandle GamePathTimerHandle;
	void GeneratePathForGridCharacters();
	void FindClosestEnemies();
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	 

};
