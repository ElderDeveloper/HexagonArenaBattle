// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "IlluviumArenaGameMode.h"

#include "Illuvium/Character/GridCharacter.h"
#include "Illuvium/Grid/HexGridManager.h"
#include "Illuvium/Grid/HexTile.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"



AIlluviumArenaGameMode::AIlluviumArenaGameMode()
{
	HexGridManager = nullptr;
	GridCharacterClass = AGridCharacter::StaticClass();
	HexGridManagerClass = AHexGridManager::StaticClass();
	GridCharacterCount = 10;
	TeamCharacterCount = FIntPoint(0,0);
}


void AIlluviumArenaGameMode::SpawnHexGridManager()
{
	if (HexGridManagerClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		HexGridManager = GetWorld()->SpawnActor<AHexGridManager>(HexGridManagerClass,SpawnParams);
		if (HasAuthority())
		{
			OnRep_HexGridManager();
		}
	}
}


void AIlluviumArenaGameMode::SpawnGridCharacters()
{
	if (HexGridManager)
	{
		for (uint8 i = 0; i < GridCharacterCount; i++)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			if (AGridCharacter* GridCharacter = GetWorld()->SpawnActor<AGridCharacter>(GridCharacterClass,SpawnParams))
			{
				GridCharacter->SetGridManager(HexGridManager);
				GridCharacters.Add(GridCharacter);
				AddCharacterToTeam(GridCharacter);
				if (AHexTile* Tile = HexGridManager->GetRandomHexTile())
				{
					GridCharacter->AssignTile(Tile);
					Tile->SetOccupyingActor(GridCharacter);
				}
				GridCharacter->SetMaxHealth(UKismetMathLibrary::RandomIntegerInRange(CharacterMinHealth,CharacterMaxHealth));
			}
		}
	}
}


void AIlluviumArenaGameMode::RemoveCharacterFromList(AGridCharacter* Character)
{
	if (Character)
	{
		if (Character->GetTeam() == EGridCharacterTeam::TeamBlue)
		{
			BlueTeamCharacters.Remove(Character);
			TeamCharacterCount.X--;
		}
		else
		{
			RedTeamCharacters.Remove(Character);
			TeamCharacterCount.Y--;
		}
		GridCharacters.Remove(Character);
	}

	// Immediately generate path for remaining characters to avoid null references
	GeneratePathForGridCharacters();
}


void AIlluviumArenaGameMode::OnRep_HexGridManager()
{
	if (HexGridManager)
	{
		HexGridManager->GenerateHexGrid();
	}
}


void AIlluviumArenaGameMode::AddCharacterToTeam(AGridCharacter* Character)
{
	if (Character)
	{
		if (TeamCharacterCount.X <= TeamCharacterCount.Y)
		{
			Character->AddPlayerToTeam(EGridCharacterTeam::TeamBlue);
			TeamCharacterCount.X++;
			BlueTeamCharacters.Add(Character);
		}
		else
		{
			Character->AddPlayerToTeam(EGridCharacterTeam::TeamRed);
			TeamCharacterCount.Y++;
			RedTeamCharacters.Add(Character);
		}
	}
}


void AIlluviumArenaGameMode::GeneratePathForGridCharacters()
{
	FindClosestEnemies();
	
	for (const auto& GridCharacter : GridCharacters)
	{
		if (GridCharacter)
		{
			if (GridCharacter->GetClosestEnemy())
			{
				TArray<AHexTile*> Path = HexGridManager->FindPathWithTilesWithAvoidance(GridCharacter->GetCurrentHexTile(), GridCharacter->GetClosestEnemy()->GetCurrentHexTile());
				GridCharacter->SetPath(Path);
			}
		}
	}
}


void AIlluviumArenaGameMode::FindClosestEnemies()
{
	FString CharacterNum = FString::FromInt(GridCharacters.Num());
	for (const auto& GridCharacter : GridCharacters)
	{
		if (!GridCharacter)
			continue;
		if (GridCharacter->IsPendingKillPending())
			continue;
		
		AGridCharacter* ClosestEnemy = nullptr;
		float ClosestDistance = TNumericLimits<float>::Max();

		if (GridCharacter->GetTeam() == EGridCharacterTeam::TeamBlue)
		{
			for (const auto& RedTeamCharacter : RedTeamCharacters)
			{
				if (RedTeamCharacter)
				{
					float Distance = FVector::DistSquared(GridCharacter->GetActorLocation(), RedTeamCharacter->GetActorLocation());
					if (Distance < ClosestDistance)
					{
						ClosestDistance = Distance;
						ClosestEnemy = RedTeamCharacter;
					}
				}
			}
		}

		if (GridCharacter->GetTeam() == EGridCharacterTeam::TeamRed)
		{
			for (const auto& BlueTeamCharacter : BlueTeamCharacters)
			{
				if (BlueTeamCharacter)
				{
					float Distance = FVector::DistSquared(GridCharacter->GetActorLocation(), BlueTeamCharacter->GetActorLocation());
					if (Distance < ClosestDistance)
					{
						ClosestDistance = Distance;
						ClosestEnemy = BlueTeamCharacter;
					}
				}
			}
		}

		GridCharacter->SetClosestEnemy(ClosestEnemy);
	}
}


void AIlluviumArenaGameMode::BeginPlay()
{
	Super::BeginPlay();

	SpawnHexGridManager();
	SpawnGridCharacters();
	GetWorldTimerManager().SetTimer(GamePathTimerHandle,this,&AIlluviumArenaGameMode::GeneratePathForGridCharacters,PathLoopInterval,true);
	
	FTimerHandle Delay;
	GetWorld()->GetTimerManager().SetTimer(Delay, [this]()
	{

	}, 3.f, false);

}


void AIlluviumArenaGameMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AIlluviumArenaGameMode, HexGridManager);
	DOREPLIFETIME(AIlluviumArenaGameMode, GridCharacters);
	DOREPLIFETIME(AIlluviumArenaGameMode, TeamCharacterCount);
	DOREPLIFETIME(AIlluviumArenaGameMode, BlueTeamCharacters);
	DOREPLIFETIME(AIlluviumArenaGameMode, RedTeamCharacters);
}
