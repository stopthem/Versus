// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VersusPlayerController.generated.h"

class USpringArmComponent;
class UCameraComponent;

/**
 *
 */
UCLASS()
class VERSUS_API AVersusPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AVersusPlayerController();
#pragma endregion
};
