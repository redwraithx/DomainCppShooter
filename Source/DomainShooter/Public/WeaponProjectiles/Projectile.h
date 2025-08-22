// Domain Shooter By Brian

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

// forward declarations
class USphereComponent;
class UProjectileMovementComponent;


UCLASS()
class DOMAINSHOOTER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnComponentOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
    	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:

	// Components
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USphereComponent> SphereCollision;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;


	// Variables
	UPROPERTY(EditAnywhere, Category = "Effects")
	TObjectPtr<UParticleSystem> ImpactParticle;

	UPROPERTY(EditAnywhere, Category = "Effects")
	TObjectPtr<USoundBase> ImpactSound;

	UPROPERTY(EditAnywhere, Category = "Damage")
	float BaseDamage = 20.f;
	
};
