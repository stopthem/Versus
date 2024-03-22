// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VersusTankProjectile.generated.h"

class UProjectileMovementComponent;

UCLASS()
class VERSUS_API AVersusTankProjectile : public AActor
{
	GENERATED_BODY()

public:
	AVersusTankProjectile();

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent = nullptr;

public:
	void NotifyActorBeginOverlap(AActor* OverlappedActor) override;
};
