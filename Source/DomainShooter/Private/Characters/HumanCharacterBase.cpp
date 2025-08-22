// Domain Shooter By Brian


//#include "Characters/HumanCharacterBase.h"

#include "DomainShooter/Public/Characters/HumanCharacterBase.h"

#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Characters/AI/DomainAiBaseController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapons/Weapon.h"
#include "Characters/AI/DomainAiBaseController.h"
#include "BrainComponent.h"


AHumanCharacterBase::AHumanCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
	
	
}

void AHumanCharacterBase::BeginPlay()
{
	Super::BeginPlay();

		
}


void AHumanCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHumanCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AHumanCharacterBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitMontage)
	{
		AnimInstance->Montage_Play(HitMontage);
	}
	
	Health -= DamageAmount;

	Health = FMath::Clamp(Health, 0.f, MaxHealth);

	//UE_LOG(LogTemp, Warning, TEXT("TakeDamage: %.2f"), DamageAmount);
	//UE_LOG(LogTemp, Warning, TEXT("PlayerHealth: %.2f, Status: %s"), Health, Health <= 0.f ? TEXT("Dead") : TEXT("Alive"));

	if (Health <= 0.f)
	{
		//UE_LOG(LogTemp, Error, TEXT("Character Dies"));

		bIsDead = true;

		// Drop Weapon to the Ground
		OnDeath();

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		if (Controller)
		{
			if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
			{
				DisableInput(PlayerController);
			}

			if (ADomainAiBaseController* AiController = Cast<ADomainAiBaseController>(Controller))
			{
				AiController->BrainComponent->StopLogic(TEXT("Character Died"));
			}
			
			Controller->SetIgnoreMoveInput(true);
			Controller->SetIgnoreLookInput(true);
		}

		 
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));

		if (UWorld* World = GetWorld())
		{
			//May not need trying to fix hands when ragdolled
			if (AnimInstance)
			{
				AnimInstance->Montage_Stop(0.1f, nullptr);
			}
			
			
			World->GetTimerManager().SetTimer(DeathTimerHandle, this, &AHumanCharacterBase::DestroyActor, DestroyTime, false);
		}
	}
	
	return DamageAmount;
}


void AHumanCharacterBase::PickupWeapon(AWeapon* WeaponToPickup, FName SocketName)
{
	WeaponToPickup->GetCollisionSphere()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponToPickup->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, SocketName);
	WeaponToPickup->SetActorRelativeLocation(FVector(0.f, 0.f, 0.f));
	WeaponToPickup->SetActorRelativeRotation(FRotator(0.0, 0.0, 0.0));
	WeaponToPickup->SetOwner(this);
}

void AHumanCharacterBase::Shoot(AWeapon* WeaponToShoot)
{
	if (WeaponToShoot)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && FireMontage)
		{
			AnimInstance->Montage_Play(FireMontage);
		}

		WeaponToShoot->WeaponShoot();
	}
}

void AHumanCharacterBase::DestroyActor()
{
	
	
	Destroy();
}

void AHumanCharacterBase::OnDeath()
{
	if (Weapon)
	{
		Weapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		Weapon->SetActorRelativeRotation(FRotator(0.f, 0.f, 0.f));
		// trace down to find ground and set location
		FHitResult HitResult;
		FVector StartLocation = GetActorLocation();
		FVector EndLocation = StartLocation - FVector(0.f, 0.f, 1000.f); // trace down 1000 units
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this); // ignore self in trace
		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, CollisionParams))
		{
			FVector GroundLocation = HitResult.Location + FVector(0.f, 0.f, 10.f);;
			Weapon->SetActorLocation(GroundLocation);
		}

		Weapon->SetOwner(nullptr);
	}
}
