// Domain Shooter By Brian


#include "Weapons/Weapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "Characters/HumanCharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "DomainShooter/Public/Characters/HumanCharacterBase.h"
#include "WeaponProjectiles/Projectile.h"


// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	RootComponent = CollisionSphere;
	CollisionSphere->SetSphereRadius(64.f);
	
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(GetRootComponent());
	WeaponMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	ProjectileLocation = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileLocation"));
	ProjectileLocation->SetupAttachment(WeaponMesh);
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	// setup collision events to handle overlaps going to custom functions
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnComponentOverlap);
	CollisionSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnComponentEndOverlap);
}


// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AHumanCharacterBase* BaseCharacter = Cast<AHumanCharacterBase>(OtherActor);
	if (BaseCharacter)
	{
		BaseCharacter->SetCanPickup(true);
		BaseCharacter->SetWeapon(this);
	}
}

void AWeapon::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AHumanCharacterBase* BaseCharacter = Cast<AHumanCharacterBase>(OtherActor);
	if (BaseCharacter)
	{
		BaseCharacter->SetCanPickup(false);
		//BaseCharacter->SetWeapon(nullptr);
	}
}

void AWeapon::WeaponShoot()
{
	UAnimInstance* AnimInstance = WeaponMesh->GetAnimInstance();
	UWorld* World = GetWorld();
	
	if (AnimInstance && FireMontage && Projectile && World)
	{
		AnimInstance->Montage_Play(FireMontage);

		FVector ProjectileSpawnLocation = ProjectileLocation->GetComponentLocation();
		FRotator ProjectileSpawnRotation = ProjectileLocation->GetComponentRotation() + FRotator(0.f, 90.f, 0.f);

		AHumanCharacterBase* BaseCharacter = Cast<AHumanCharacterBase>(GetOwner());

		if (BaseCharacter)
		{
			if (APlayerController* PlayerController = Cast<APlayerController>(BaseCharacter->GetController()))
			{
				FVector PlayerViewLocation;
				FRotator PlayerViewRotation;
				PlayerController->GetPlayerViewPoint(PlayerViewLocation, PlayerViewRotation);

				FVector EndLocation = PlayerViewLocation + (PlayerViewRotation.Vector() * TargetDistance);
				
				FHitResult Hit;
				if (World->LineTraceSingleByChannel(Hit, PlayerViewLocation, EndLocation, ECollisionChannel::ECC_Visibility))
				{
					FVector HitLocation = Hit.ImpactPoint;
					//DrawDebugLine(World, PlayerViewLocation, HitLocation, FColor::Red, false, 5.f);

					FRotator TargetShotRotation = UKismetMathLibrary::FindLookAtRotation(ProjectileSpawnLocation, HitLocation);

					FActorSpawnParameters SpawnParams;
					SpawnParams.Instigator = Cast<APawn>(GetOwner());
					SpawnParams.Owner = this;
					
					World->SpawnActor<AProjectile>(Projectile, ProjectileSpawnLocation, TargetShotRotation, SpawnParams);
				}
				// could add else so you can shoot even if you dont hit anything
			}
			else
			{
				World->SpawnActor<AProjectile>(Projectile, ProjectileSpawnLocation, ProjectileSpawnRotation);
			}
		}
		
		
	}
}
