// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "VersusPlayerState.generated.h"

/**
 * AVersusPlayerState
 * 
 * Player State class that handles score for specific AVersusCharacters
 */
UCLASS()
class VERSUS_API AVersusPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AVersusPlayerState();

protected:
	// Create a timer to update score in ui
	// We do this because when a player joins a game, this gets called
	void BeginPlay() override;

#pragma region Score
public:
	// Update UI when score is changed
	void OnRep_Score() override;

	// Network Decider. Updates score in local first then
	// if client, tells server to do same thing
	void ND_IncreaseScore(float IncreaseAmount = 1.0f);
private:
	// Update score variable in server
	UFUNCTION(Server, Reliable, WithValidation)
	void SVR_IncreaseScore(float IncreaseAmount = 1.0f);

	// Uses adds IncreaseAmount to GetScore() and gives it to SetScore()
	void IncreaseScore(float IncreaseAmount = 1.0f);
#pragma endregion
};
