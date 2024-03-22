// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "VersusHUD.generated.h"

/**
 * 
 */
UCLASS()
class VERSUS_API AVersusHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	AVersusHUD();

public:
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateScores();
};
