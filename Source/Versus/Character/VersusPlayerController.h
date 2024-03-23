// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VersusPlayerController.generated.h"

class USpringArmComponent;
class UCameraComponent;

/**
 * AVersusPlayerController
 * 
 * Player Controller class that posseses AVersusCharacter
 */
UCLASS()
class VERSUS_API AVersusPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AVersusPlayerController();

public:
	// Fix path following component when pawn is replicated to us(spawn)
	void OnRep_Pawn() override;


#pragma endregion
};
