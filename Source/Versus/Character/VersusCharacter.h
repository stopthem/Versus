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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	AVersusPlayerController* GetVersusPlayerController() const
	{
		return Cast<AVersusPlayerController>(GetController());
	}

	AVersusPlayerState* GetVersusPlayerState() const
	{
		return Cast<AVersusPlayerState>(GetPlayerState());
	}

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

	UPROPERTY(EditDefaultsOnly, Category = "VersusCharacter|Input")
	TObjectPtr<UInputAction> MoveToMouseLocationAction = nullptr;

	void OnMoveToMouseLocationAction(const FInputActionInstance& InputActionInstance);

	void ND_MoveToMouseLocation();

	UFUNCTION(Server, Reliable, WithValidation)
	void SVR_SimpleMoveToLocation(const FVector& Location);

	UPROPERTY(EditDefaultsOnly, Category = "VersusCharacter|Input")
	TObjectPtr<UInputAction> ShootAction = nullptr;

	void OnShootAction(const FInputActionInstance& InputActionInstance);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
#pragma endregion

#pragma region Shoot
private:
	bool bCanShoot = true;

private:
	UPROPERTY(EditDefaultsOnly, Category = "VersusCharacter|Shoot")
	TSubclassOf<AVersusTankProjectile> ProjectileClassToShoot;

	UPROPERTY(EditDefaultsOnly, Category = "VersusCharacter|Shoot", meta = (ClampMin = "0", Units = "s"))
	float ShootCooldown = 1.25f;

	UPROPERTY(EditDefaultsOnly, Category = "VersusCharacter|Shoot")
	TObjectPtr<USceneComponent> ShootLocationSceneComponent = nullptr;

private:
	void ND_Shoot();

	UFUNCTION(Server, Reliable, WithValidation)
	void SVR_Shoot();

	void AUTH_Shoot();
#pragma endregion

public:
	void AUTH_OnHitByProjectile(const AVersusTankProjectile* VersusTankProjectile);
};
