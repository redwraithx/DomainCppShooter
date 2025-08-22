// Domain Shooter By Brian

#pragma once

#include "CoreMinimal.h"
#include "Characters/HumanCharacterBase.h"
#include "PlayerCharacter.generated.h"


// forward declarations
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class USpringArmComponent;
class UCameraComponent;
class AWeapon;
class UAnimMontage;
class UUserWidget;
class UAIPerceptionStimuliSourceComponent;
class UPawnNoiseEmitterComponent;


USTRUCT(BlueprintType)
struct FHealthBarParams
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	float Health;
	
	UPROPERTY(BlueprintReadWrite)
	float MaxHealth;
};

UCLASS()
class DOMAINSHOOTER_API APlayerCharacter : public AHumanCharacterBase
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	void GetUpdateHealthBarParams(FHealthBarParams& Params);

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	// Character Components
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> BaseSpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	TObjectPtr<UAIPerceptionStimuliSourceComponent> PerceptionStimulusSource;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	TObjectPtr<UPawnNoiseEmitterComponent> NoiseEmitter;
	
	// Input Mapping Context
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> Imc_PlayerCharacterInputs;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> Ia_PlayerCharacterMovement;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> Ia_PlayerCharacterLookAround;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> Ia_PlayerCharacterJump;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> Ia_PlayerCharacterPickupWeapon;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> Ia_PlayerCharacterShoot;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> Ia_PlayerCharacterCameraZoom;

	// Input Functions
	void CharacterMovement(const FInputActionValue& InputActionValue);
	void CharacterLookAround(const FInputActionValue& InputActionValue);
	void CharacterJump(const FInputActionValue& InputActionValue);
	void CharacterPickupWeapon(const FInputActionValue& InputActionValue);
	void CharacterShoot(const FInputActionValue& InputActionValue);
	void CharacterCameraZoomIn(const FInputActionValue& InputActionValue);
	void CharacterCameraZoomOut(const FInputActionValue& InputActionValue);

	
	// UI
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> CharacterUI;

	TObjectPtr<UUserWidget> UIWidget;


	// Weapon Variables
	UPROPERTY(EditAnywhere, Category = "Weapon")
	float CameraZoomedFOV = 35.f;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	float CameraDefaultFOV = 90.f;



	// Character Variables
	UPROPERTY(EditAnywhere, Category = "Character|Settings")
	float LookAroundSpeed = 1.f;

	UPROPERTY(EditAnywhere, Category = "Character|Settings", meta = (ClampMin = "0.1", ClampMax = "1.25"))
	float DefaultLookAroundSpeed = 1.f;
	
	UPROPERTY(EditAnywhere, Category = "Character|Settings", meta = (ClampMin = "0.1", ClampMax = "1.25"))
	float ZoomedLookAroundSpeed = 0.5f;


	// Character Functions
	UFUNCTION()
	void SetLookAroundSpeed(float NewLookAroundSpeed);
	void UpdateHealthBarUI();

	virtual void OnDeath() override;

	
	
	
	
public:	
	// Setters and Getters
	UFUNCTION(BlueprintPure, Category = "Health")
	FORCEINLINE float GetHealth() const { return Health; }

	UFUNCTION(BlueprintPure, Category = "Health")
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

};

