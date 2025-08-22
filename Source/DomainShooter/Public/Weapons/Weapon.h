// Domain Shooter By Brian

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

// forward declarations
class USkeletalMeshComponent;
class USphereComponent;
class AProjectile; 

UCLASS()
class DOMAINSHOOTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;


protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	void WeaponShoot();

private:
	/// Weapon Components
	UPROPERTY(VisibleAnywhere, Category = "Weapon Components")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	UPROPERTY(EditAnywhere, Category = "Weapon Components")
	TObjectPtr<USphereComponent> CollisionSphere;

	UPROPERTY(EditAnywhere, Category = "Weapon Components")
	TObjectPtr<USceneComponent> ProjectileLocation;

	UPROPERTY(EditAnywhere, Category = "Animations")
	TObjectPtr<UAnimMontage> FireMontage;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	TSubclassOf<AProjectile> Projectile;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	float TargetDistance = 20000.f;
	
public:	
	// Setters and Getters
	FORCEINLINE USphereComponent* GetCollisionSphere() const { return CollisionSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }

	
};
