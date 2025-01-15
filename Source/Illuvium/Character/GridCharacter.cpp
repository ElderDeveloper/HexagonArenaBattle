// Fill out your copyright notice in the Description page of Project Settings.

#include "GridCharacter.h"
#include "Illuvium/Grid/HexTile.h"
#include "Illuvium/Grid/HexGridManager.h"
#include "Kismet/KismetMathLibrary.h"
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


void AGridCharacter::OnRep_Enemy()
{
}


void AGridCharacter::OnRep_Path()
{
	// When Path Num is 1 it means the character is on the destination tile
	if (Path.Num() > 2 )
	{
		if (CurrentHexTile)
		{
			CurrentHexTile->ClearOccupyingActor();
		}
		CurrentHexTile = Path[1];
		if (CurrentHexTile)
		{
			CurrentHexTile->SetOccupyingActor(this);
			//SetActorLocation(CurrentHexTile->GetActorLocation() + CharacterRelativePositionOnTile);
			TargetGridLocation = CurrentHexTile->GetActorLocation() + CharacterRelativePositionOnTile;
			StartLocation = GetActorLocation();
			Alpha = 0.f;
		}
	}
}


void AGridCharacter::BeginPlay()
{
	Super::BeginPlay();
}


void AGridCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority() && TargetGridLocation != FVector::ZeroVector)
	{
		Alpha = FMath::Clamp(Alpha+DeltaTime*9/10, 0.f, 1.f);
		SetActorLocation(UKismetMathLibrary::VLerp(StartLocation,TargetGridLocation,Alpha));
	}
	if (CurrentHexTile)
	{
		DrawDebugSphere(GetWorld(), CurrentHexTile->GetActorLocation(), 50.f, 12, FColor::Green, false, 0.1f);
	}
}


void AGridCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGridCharacter, ClosestEnemy);
	DOREPLIFETIME(AGridCharacter, Path);
	DOREPLIFETIME(AGridCharacter, Team);
}
