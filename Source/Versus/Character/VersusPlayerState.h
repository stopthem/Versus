// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "VersusPlayerState.generated.h"

/**
 *
 */
UCLASS()
class VERSUS_API AVersusPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AVersusPlayerState();

#pragma region Score
public:
	void OnRep_Score() override;

	void ND_IncreaseScore(float IncreaseAmount = 1.0f);

private:
	UFUNCTION(Server, Reliable, WithValidation)
	void SVR_IncreaseScore(float IncreaseAmount = 1.0f);

	void IncreaseScore(float IncreaseAmount = 1.0f);
#pragma endregion
};
