// Fill out your copyright notice in the Description page of Project Settings.

#include "GridCharacter.h"

#include "Illuvium/GameMode/IlluviumArenaGameMode.h"
#include "Illuvium/Grid/HexTile.h"
#include "Illuvium/Grid/HexGridManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AGridCharacter::AGridCharacter()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
}



void AGridCharacter::AssignTile(AHexTile* Tile)
{
	if (Tile)
	{
		CurrentHexTile = Tile;
		SetActorLocation(Tile->GetActorLocation() + CharacterRelativePositionOnTile);
	}
}


void AGridCharacter::OnRep_Team()
{
	OnTeamChanged(Team);
}


void AGridCharacter::AssignCurrentHexTile(bool ClearCurrentTile)
{
	// When Path Num is 1 it means the character is on the destination tile
	if (Path.Num() > 2)
	{
		if (CurrentHexTile && ClearCurrentTile)
		{
			CurrentHexTile->ClearOccupyingActor();
		}
		CurrentHexTile = Path[1];
		if (CurrentHexTile)
		{
			CurrentHexTile->SetOccupyingActor(this);
		}
	}
	else if (Path.Num() >= 1)
	{
		CurrentHexTile = Path[0];
	}
}


bool AGridCharacter::CheckNeighbourTilesForEnemy()
{
	if (HexGridManager && CurrentHexTile && ClosestEnemy)
	{
		TArray<AHexTile*> NeighbourHexTiles = HexGridManager->GetNeighbourHexTiles(CurrentHexTile->HexGridPosition);
		for (AHexTile* Tile : NeighbourHexTiles)
		{
			if (Tile->GetOccupyingActor() && Tile->GetOccupyingActor() == ClosestEnemy)
			{
				if (!GetWorld()->GetTimerManager().IsTimerActive(ApplyDamageTimerHandle))
				{
					GetWorld()->GetTimerManager().SetTimer( ApplyDamageTimerHandle, this, &AGridCharacter::ApplyDamageToClosestEnemy, 3.f, true);
				}
				return true;
			}
		}
	}
	if (GetWorld()->GetTimerManager().IsTimerActive(ApplyDamageTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(ApplyDamageTimerHandle);
	}
	return false;
}


void AGridCharacter::SetPath(const TArray<AHexTile*>& NewPath)
{
	Path = NewPath;
	if(HasAuthority())
		OnRep_Path();
}


void AGridCharacter::OnRep_Path()
{
	AssignCurrentHexTile(true);
	
	if (CurrentHexTile)
	{
		StartLocation = GetActorLocation();
		TargetGridLocation = CurrentHexTile->GetActorLocation() + CharacterRelativePositionOnTile;

		if (HasAuthority())
		{
			SetActorLocation(TargetGridLocation);
		}
		CheckNeighbourTilesForEnemy();
	}
}


void AGridCharacter::OnRep_CurrentHealth()
{
	if (CurrentHealth <= 0)
	{
		if (CurrentHexTile)
		{
			CurrentHexTile->ClearOccupyingActor();
		}
		Destroy();
	}
	else
	{
		OnTookDamage(LastTakenDamage,CurrentHealth,MaxHealth);
	}
	
}


void AGridCharacter::ApplyDamageToClosestEnemy()
{
	if (CheckNeighbourTilesForEnemy())
	{
		ClosestEnemy->ReceiveDamage(1);
		OnApplyDamage(ClosestEnemy,1);
	}
}


void AGridCharacter::ReceiveDamage(uint8 DamageTaken)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - DamageTaken, 0, MaxHealth);
	LastTakenDamage = DamageTaken;
	if (HasAuthority())
	{
		if (CurrentHealth <= 0)
		{
			if (const auto GM = Cast<AIlluviumArenaGameMode>(GetWorld()->GetAuthGameMode()))
			{
				GM->RemoveCharacterFromList(this);
			}
		}
		OnRep_CurrentHealth();
	}
}


void AGridCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentHexTile)
	{
		if (!HasAuthority())
		{
			FVector ForwardVector = TargetGridLocation-GetActorLocation();
			if (ForwardVector.SquaredLength() > 20.f)
			{
				AddActorLocalOffset(ForwardVector.GetSafeNormal()*DeltaTime*MoveSpeed);
			}
		}
	}
}


void AGridCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGridCharacter, ClosestEnemy);
	DOREPLIFETIME(AGridCharacter, Path);
	DOREPLIFETIME(AGridCharacter, Team);
	DOREPLIFETIME(AGridCharacter, CurrentHealth);
	DOREPLIFETIME(AGridCharacter, LastTakenDamage);
}
