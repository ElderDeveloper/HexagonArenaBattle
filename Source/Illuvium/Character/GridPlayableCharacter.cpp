// Fill out your copyright notice in the Description page of Project Settings.


#include "GridPlayableCharacter.h"
#include "EnhancedInputComponent.h"


// Sets default values
AGridPlayableCharacter::AGridPlayableCharacter()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AGridPlayableCharacter::Move(const FInputActionValue& Value)
{
}

// Called when the game starts or when spawned
void AGridPlayableCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGridPlayableCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGridPlayableCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (const auto EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGridPlayableCharacter::Move);
	}
}

