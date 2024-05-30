// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "VersusHUD.generated.h"

/**
 * AVersusHUD
 * 
 * HUD class that handles Score UI
 */
UCLASS()
class VERSUS_API AVersusHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	AVersusHUD();

public:
	// Update player scores
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateScores();
};
