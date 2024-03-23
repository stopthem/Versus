// Fill out your copyright notice in the Description page of Project Settings.


#include "VersusPlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Camera/CameraComponent.h"
#include "Versus/HUD/VersusHUD.h"
#include "Kismet/GameplayStatics.h"

AVersusPlayerController::AVersusPlayerController()
{
	bShowMouseCursor = true;
}

void AVersusPlayerController::OnRep_Pawn()
{
	// UPathFollowingComponent registers only to server - only delegates for detect pawn changes
	// Invoke the same functionality on the client - side

	if (!GetPawn())
	{
		return;
	}

	if (HasAuthority())
	{
		return;
	}

	UPathFollowingComponent* PathFollowingComponent = FindComponentByClass<UPathFollowingComponent>();
	if (!PathFollowingComponent)
	{
		return;
	}

	PathFollowingComponent->UpdateCachedComponents();
}
