#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VersusPlayerController.h"
#include "VersusPlayerState.h"
#include "VersusCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionInstance;
class AVersusTankProjectile;
class UBoxComponent;

/*
* AVersusCharacter
*
* Class that all the players in the game uses
* Handles input, shooting
*/
UCLASS()
class VERSUS_API AVersusCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVersusCharacter();

public:
	AVersusPlayerController* GetVersusPlayerController() const
	{
		return Cast<AVersusPlayerController>(GetController());
	}

	AVersusPlayerState* GetVersusPlayerState() const
	{
		return Cast<AVersusPlayerState>(GetPlayerState());
	}

private:
	// Box component that we use more precise collision for
	// our tank
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> BoxComponentForCollision = nullptr;

#pragma region Camera
private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<USpringArmComponent> CameraSpringArmComponent = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UCameraComponent> CameraComponent = nullptr;

#pragma endregion
#pragma region Input
private:

	UPROPERTY(EditDefaultsOnly, Category = "VersusCharacter|Input")
	TObjectPtr<UInputMappingContext> MappingContext = nullptr;

#pragma region Move To Mouse Location Input
	// Input action that enables us to move under cursor location
	UPROPERTY(EditDefaultsOnly, Category = "VersusCharacter|Input")
	TObjectPtr<UInputAction> MoveToMouseLocationAction = nullptr;

	// Called when MoveToMouseLocationAction is triggered
	void OnMoveToMouseLocationAction(const FInputActionInstance& InputActionInstance);

	// Network Decider. If called from client, call server rpc and
	// do the actual moving logic in client too before server
	void ND_MoveToMouseLocation();

#pragma endregion

#pragma region Shoot Input
	// Move to location in server too
	UFUNCTION(Server, Reliable, WithValidation)
	void SVR_SimpleMoveToLocation(const FVector& Location);

	// Input action that enables us to shoot
	UPROPERTY(EditDefaultsOnly, Category = "VersusCharacter|Input")
	TObjectPtr<UInputAction> ShootAction = nullptr;

	// Called when ShootAction is triggered
	void OnShootAction(const FInputActionInstance& InputActionInstance);
#pragma endregion

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
#pragma endregion

#pragma region Shoot
private:
	bool bCanShoot = true;

private:
	// Projectile class that we will spawn when shooting
	UPROPERTY(EditDefaultsOnly, Category = "VersusCharacter|Shoot")
	TSubclassOf<AVersusTankProjectile> ProjectileClassToShoot;

	UPROPERTY(EditDefaultsOnly, Category = "VersusCharacter|Shoot", meta = (ClampMin = "0", Units = "s"))
	float ShootCooldown = 1.25f;

	// SceneComponent that we use its transform to spawn our projectile
	UPROPERTY(EditDefaultsOnly, Category = "VersusCharacter|Shoot")
	TObjectPtr<USceneComponent> ShootLocationSceneComponent = nullptr;

private:
	// Network Decider. If called from client, rpc server and shoot there
	// if not, shoot directly
	void ND_Shoot();

	// Shoot in server
	UFUNCTION(Server, Reliable, WithValidation)
	void SVR_Shoot();

	// Authority only! Do the actual shooting
	void AUTH_Shoot();
#pragma endregion

public:
	// Called when a projectile is overlapped us and killed us
	void AUTH_OnHitByProjectile(const AVersusTankProjectile* VersusTankProjectile);
};
