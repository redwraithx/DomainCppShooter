// Domain Shooter By Brian

#pragma once

#include "CoreMinimal.h"
#include "Characters/HumanCharacterBase.h"
#include "AICharacter.generated.h"



// forward declarations
class AWeapon;



UCLASS()
class DOMAINSHOOTER_API AAICharacter : public AHumanCharacterBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	void ShootTask();

private:
	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<AWeapon> WeaponClass;
	
};
