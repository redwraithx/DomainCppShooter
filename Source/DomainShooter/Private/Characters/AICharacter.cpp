// Domain Shooter By Brian


#include "Characters/AICharacter.h"
#include "Weapons/Weapon.h"


void AAICharacter::BeginPlay()
{
	Super::BeginPlay();

	if (WeaponClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("AI picked up weapon"));

		UWorld* World = GetWorld();
		if (World && !Weapon)
		{
			Weapon = World->SpawnActor<AWeapon>(WeaponClass);

			if (Weapon)
			{
				PickupWeapon(Weapon, TEXT("AIWeaponSocket"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to spawn weapon for AI character"));
			}
		}
	}	

		

}

void AAICharacter::ShootTask()
{
	Shoot(Weapon);
}
