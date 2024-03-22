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

// Sets default values
AVersusCharacter::AVersusCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	ShootLocationSceneComponent = CreateDefaultSubobject<USceneComponent>("ShootLocationSceneComponent");
	ShootLocationSceneComponent->SetupAttachment(GetRootComponent());

	CameraSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("CameraSpringArmComponent");
	CameraSpringArmComponent->SetupAttachment(GetRootComponent());
	CameraSpringArmComponent->bEnableCameraLag = true;
	CameraSpringArmComponent->CameraLagSpeed = 10.0f;
	CameraSpringArmComponent->SetRelativeRotation(FRotator(-50, 0, 0));
	CameraSpringArmComponent->TargetArmLength = 400.0f;
	CameraSpringArmComponent->bInheritYaw = false;
	CameraSpringArmComponent->bDoCollisionTest = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(CameraSpringArmComponent);

	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;

	bReplicates = true;
}

// Called when the game starts or when spawned
void AVersusCharacter::BeginPlay()
{
	Super::BeginPlay();
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
	ND_MoveToMouseLocation();
}

void AVersusCharacter::ND_MoveToMouseLocation()
{
	FHitResult HitResult;
	GetVersusPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, HitResult);

	if (!HitResult.bBlockingHit)
	{
		return;
	}

	UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), HitResult.Location);

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
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), Location);
}

void AVersusCharacter::OnShootAction(const FInputActionInstance& InputActionInstance)
{
	if (!bCanShoot)
	{
		return;
	}

	bCanShoot = false;

	FTimerHandle ShootCooldownTimerHandle;
	GetWorldTimerManager().SetTimer(ShootCooldownTimerHandle, FTimerDelegate::CreateLambda([&]
		{
			bCanShoot = true;
		}), ShootCooldown, false);

	ND_Shoot();
}

void AVersusCharacter::ND_Shoot()
{
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
	ProjectileSpawnParams.Instigator = this;

	AVersusTankProjectile* TankProjectile = GetWorld()->SpawnActor<AVersusTankProjectile>(ProjectileClassToShoot, ShootLocationSceneComponent->GetComponentTransform(), ProjectileSpawnParams);
}

void AVersusCharacter::AUTH_OnHitByProjectile(const AVersusTankProjectile* VersusTankProjectile)
{
	Destroy();
}
