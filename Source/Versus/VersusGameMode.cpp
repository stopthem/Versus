// Copyright Epic Games, Inc. All Rights Reserved.


#include "VersusGameMode.h"
#include "Kismet//GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "NavigationSystem.h"

AVersusGameMode::AVersusGameMode()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AVersusGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	// Spawn player for each new joined player
	SpawnPlayer(NewPlayer);
}

APawn* AVersusGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	// Don't want default pawn spawning automatically
	return nullptr;
}

void AVersusGameMode::SpawnPlayer(AController* Controller)
{
	// Find random reachable point in radius
	FNavLocation FoundReachableLocation;
	UNavigationSystemV1* NavigationSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);
	NavigationSystem->GetRandomReachablePointInRadius(FVector::ZeroVector, RandomReachablePointRadius, FoundReachableLocation);

	// We want to always spawn
	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Spawn the player in the found point
	APawn* SpawnedPlayer = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, FoundReachableLocation.Location, FRotator::ZeroRotator, ActorSpawnParameters);

	// Let controller posses spawned player
	Controller->Possess(SpawnedPlayer);
}
