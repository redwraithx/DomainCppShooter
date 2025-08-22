// Domain Shooter By Brian

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HumanCharacterBase.generated.h"

// forward declarations
class AWeapon;
class UAnimMontage;

UCLASS()
class DOMAINSHOOTER_API AHumanCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AHumanCharacterBase();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//UFUNCTION()
	//virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator);

	// call takedamage from applydamage
	UFUNCTION(BlueprintCallable, Category = "Health")
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	

protected:
	virtual void BeginPlay() override;

	// Weapon Variables
	bool bCanPickup = false;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TObjectPtr<AWeapon> Weapon;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	bool bHasRifle = false;

	

	// Animations
	UPROPERTY(EditAnywhere, Category = "Animations")
	TObjectPtr<UAnimMontage> FireMontage;

	UPROPERTY(EditAnywhere, Category = "Animations")
	TObjectPtr<UAnimMontage> HitMontage;


	// Health Variables
	UPROPERTY(VisibleAnywhere, Category = "Health")
	float Health = 100.f;

	UPROPERTY(VisibleAnywhere, Category = "Health")
	float MaxHealth = 100.f;

	UPROPERTY(BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	bool bIsDead = false;


	// Weapon Functions
	void PickupWeapon(AWeapon* WeaponToPickup, FName SocketName);
	void Shoot(AWeapon* WeaponToShoot);

private:
	
	// Death Timer
	FTimerHandle DeathTimerHandle;

	UFUNCTION()
	void DestroyActor();

	UPROPERTY(EditAnywhere, Category = "DeathTimer")
	float DestroyTime = 10.f;

	UFUNCTION()
	virtual void OnDeath();
	
public:	
	// Setters and Getters
	FORCEINLINE void SetCanPickup(const bool NewPickupState) { bCanPickup = NewPickupState; }
	FORCEINLINE void SetWeapon(AWeapon* NewWeapon) { if (!Weapon) {Weapon = NewWeapon; } }

};
