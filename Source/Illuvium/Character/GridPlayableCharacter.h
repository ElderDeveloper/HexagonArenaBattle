// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridCharacter.h"
#include "InputAction.h"
#include "GridPlayableCharacter.generated.h"

class UInputAction;

UCLASS()
class ILLUVIUM_API AGridPlayableCharacter : public AGridCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGridPlayableCharacter();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* AttackAction;

protected:
	void Move(const FInputActionValue& Value);
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
