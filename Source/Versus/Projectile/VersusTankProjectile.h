// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VersusTankProjectile.generated.h"

class UProjectileMovementComponent;

/*
* AVersusTankProjectile
* 
* Projectile Class that AVersusCharacter fires
*/
UCLASS()
class VERSUS_API AVersusTankProjectile : public AActor
{
	GENERATED_BODY()

public:
	AVersusTankProjectile();

private:
	// Component that handles our movement
	UPROPERTY(EditAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent = nullptr;

	// Root component
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent = nullptr;

public:
	// Check for any overlapped players that aren't our instigator
	void NotifyActorBeginOverlap(AActor* OverlappedActor) override;
};
