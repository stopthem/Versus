#include "VersusCharacter.h"
#include "GameFramework/PlayerController.h"
#include "VersusPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/GameplayStatics.h"
#include <EnhancedInputComponent.h>
#include <EnhancedInputSubsystems.h>
#include "InputAction.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Versus/Projectile/VersusTankProjectile.h>
#include <Versus/VersusGameMode.h>
#include "Net/UnrealNetwork.h"
#include "GameFramework/GameMode.h"
#include "Components/BoxComponent.h"

// Sets default values
AVersusCharacter::AVersusCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create shoot location scene component
	ShootLocationSceneComponent = CreateDefaultSubobject<USceneComponent>("ShootLocationSceneComponent");
	ShootLocationSceneComponent->SetupAttachment(GetRootComponent());

	// Create box component for collision
	BoxComponentForCollision = CreateDefaultSubobject<UBoxComponent>("BoxComponentForCollision");
	BoxComponentForCollision->SetupAttachment(RootComponent);
	BoxComponentForCollision->InitBoxExtent(FVector(40, 20, 20));

	// Create camera spring arm component
	CameraSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("CameraSpringArmComponent");
	CameraSpringArmComponent->SetupAttachment(GetRootComponent());
	CameraSpringArmComponent->bEnableCameraLag = true;
	CameraSpringArmComponent->CameraLagSpeed = 10.0f;
	CameraSpringArmComponent->SetRelativeRotation(FRotator(-50, 0, 0));
	CameraSpringArmComponent->TargetArmLength = 400.0f;
	CameraSpringArmComponent->bInheritYaw = false;
	CameraSpringArmComponent->bDoCollisionTest = false;

	// Create camera component and attach it to the spring arm
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(CameraSpringArmComponent);

	bReplicates = true;
}

// Called to bind functionality to input
void AVersusCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (!EnhancedInput)
	{
		return;
	}

	// Bind our shoot and move to mouse location input actions to our functions
	EnhancedInput->BindAction(MoveToMouseLocationAction, ETriggerEvent::Triggered, this, &AVersusCharacter::OnMoveToMouseLocationAction);
	EnhancedInput->BindAction(ShootAction, ETriggerEvent::Triggered, this, &AVersusCharacter::OnShootAction);

	ULocalPlayer* LocalPlayer = GetVersusPlayerController()->GetLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!InputSystem)
	{
		return;
	}

	InputSystem->AddMappingContext(MappingContext, 0);
}

void AVersusCharacter::OnMoveToMouseLocationAction(const FInputActionInstance& InputActionInstance)
{
	// When input is triggered, call network decider to shoot
	ND_MoveToMouseLocation();
}

void AVersusCharacter::ND_MoveToMouseLocation()
{
	// Get hit result under cursor
	FHitResult HitResult;
	GetVersusPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, HitResult);

	if (!HitResult.bBlockingHit)
	{
		return;
	}

	// Move in local first so we don't see any delay
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), HitResult.Location);

	// If we are client, make movement in server too so other clients are in sync
	if (!HasAuthority())
	{
		SVR_SimpleMoveToLocation(HitResult.Location);
	}
}

bool AVersusCharacter::SVR_SimpleMoveToLocation_Validate(const FVector& Location)
{
	return true;
}

void AVersusCharacter::SVR_SimpleMoveToLocation_Implementation(const FVector& Location)
{
	// Move on server
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), Location);
}

void AVersusCharacter::OnShootAction(const FInputActionInstance& InputActionInstance)
{
	if (!bCanShoot)
	{
		return;
	}

	bCanShoot = false;

	// Create shoot cooldown timer
	FTimerHandle ShootCooldownTimerHandle;
	GetWorldTimerManager().SetTimer(ShootCooldownTimerHandle, FTimerDelegate::CreateLambda([&]
		{
			bCanShoot = true;
		}), ShootCooldown, false);

	// Call network decider to actually shoot
	ND_Shoot();
}

void AVersusCharacter::ND_Shoot()
{
	// We will spawn a projectile so that has to happen in server
	// and server will replicate it to clients
	if (HasAuthority())
	{
		AUTH_Shoot();
	}
	else
	{
		SVR_Shoot();
	}
}

void AVersusCharacter::SVR_Shoot_Implementation()
{
	AUTH_Shoot();
}

bool AVersusCharacter::SVR_Shoot_Validate()
{
	return true;
}

void AVersusCharacter::AUTH_Shoot()
{
	FActorSpawnParameters ProjectileSpawnParams;
	// It is important that we give instigator as this
	// So projectile knows who it belonged to
	ProjectileSpawnParams.Instigator = this;

	// Spawn the projectile at shoot location
	AVersusTankProjectile* TankProjectile = GetWorld()->SpawnActor<AVersusTankProjectile>(ProjectileClassToShoot, ShootLocationSceneComponent->GetComponentTransform(), ProjectileSpawnParams);
}

void AVersusCharacter::AUTH_OnHitByProjectile(const AVersusTankProjectile* VersusTankProjectile)
{
	// Cancel simple move to location by giving it our current location
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), GetActorLocation());

	// Tell game mode that we have to be respawned
	Cast<AVersusGameMode>(UGameplayStatics::GetGameMode(this))->SpawnPlayer(GetController());

	Destroy();
}
