// Fill out your copyright notice in the Description page of Project Settings.


#include "VersusTankProjectile.h"
#include "GameFramework/Actor.h"
#include <Versus/Character/VersusPlayerState.h>
#include <Versus/Character/VersusCharacter.h>
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AVersusTankProjectile::AVersusTankProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	SetRootComponent(StaticMeshComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	ProjectileMovementComponent->InitialSpeed = 1000.0f;
	ProjectileMovementComponent->MaxSpeed = 1000.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;

	// When we spawned, we want server to spawn us in all clients
	bReplicates = true;

	// We are not going to replicate stuff
	NetDormancy = ENetDormancy::DORM_DormantAll;
	NetUpdateFrequency = 0;
	MinNetUpdateFrequency = 0;
}

void AVersusTankProjectile::NotifyActorBeginOverlap(AActor* OverlappedActor)
{
	Super::NotifyActorBeginOverlap(OverlappedActor);

	// We only detect collision in server because if client checks this too,
	// same things will happen twice and we don't want that. Also for accuracy
	if (!HasAuthority())
	{
		return;
	}

	// We don't want to hit our instigator
	if (OverlappedActor == GetInstigator())
	{
		return;
	}

	// If overlapped actor is a versuscharacter, kill it
	if (AVersusCharacter* OverlappedVersusCharacter = Cast<AVersusCharacter>(OverlappedActor))
	{
		OverlappedVersusCharacter->AUTH_OnHitByProjectile(this);

		Cast<AVersusCharacter>(GetInstigator())->GetVersusPlayerState()->ND_IncreaseScore();
	}

	Destroy();
}

