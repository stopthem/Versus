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
	ProjectileMovementComponent->InitialSpeed = 100.0f;
	ProjectileMovementComponent->MaxSpeed = 100.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetUpdatedComponent(StaticMeshComponent);

	bReplicates = true;
}

void AVersusTankProjectile::NotifyActorBeginOverlap(AActor* OverlappedActor)
{
	Super::NotifyActorBeginOverlap(OverlappedActor);

	if (!HasAuthority())
	{
		return;
	}

	if (OverlappedActor == GetInstigator())
	{
		return;
	}

	if (AVersusCharacter* OverlappedVersusCharacter = Cast<AVersusCharacter>(OverlappedActor))
	{
		OverlappedVersusCharacter->AUTH_OnHitByProjectile(this);

		Cast<AVersusCharacter>(GetInstigator())->GetVersusPlayerState()->ND_IncreaseScore();
	}

	Destroy();
}

