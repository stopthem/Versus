// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "VersusGameMode.generated.h"

/**
 * AVersusGameMode
 *
 * GameMode class that this project uses default
 */
UCLASS()
class VERSUS_API AVersusGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AVersusGameMode();
protected:
	// Spawn players in a reachable random point
	void OnPostLogin(AController* NewPlayer) override;

public:
	// Prevent default pawn spawning automatically
	// We will do that in On Pos Login
	APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;

public:
	// When spawning players, we use this radius to find a random reachable point
	UPROPERTY(EditDefaultsOnly, Category = "VersusGameMode|PlayerSpawning", meta = (ClampMin = "0", Units = "cm"))
	float RandomReachablePointRadius = 10000.0f;

	// Spawn player in a random reachable point from world center
	// and posses it with given Controller
	void SpawnPlayer(AController* Controller);
};
