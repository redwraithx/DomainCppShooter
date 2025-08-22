// Domain Shooter By Brian


#include "Characters/PlayerCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "DomainShooter/Public/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Blueprint/UserWidget.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"



APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	BaseSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("BaseSpringArm"));
	BaseSpringArm->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(BaseSpringArm);

	// AI Sight
	PerceptionStimulusSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("PerceptionStimulusSource"));
	PerceptionStimulusSource->RegisterForSense(UAISense_Sight::StaticClass());
	// AI Hearing
	NoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("NoiseEmitter"));
	PerceptionStimulusSource->RegisterForSense(UAISense_Hearing::StaticClass());

	// AI Register Senses
	PerceptionStimulusSource->RegisterWithPerceptionSystem();


	// A way to add Widget UI to character (I went another route but want to keep this for reference)
	/*
	// the header will need these lines to make this work

	// -- Start of Header lines --
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> Healthbar;

	TObjectPtr<UUserWidget> UIHealthbar;

	// -- End of header lines --

	
	if (Healthbar)
	{
		UIHealthBar = CreateWidget<UUserWidget>(GetWorld(), Healthbar);

		if (UIHealthBar)
		{
			UIHealthBar->AddToViewport();
		}
	}*/
	
}

void APlayerCharacter::GetUpdateHealthBarParams(FHealthBarParams &OutParams)
{ 
	OutParams.Health = Health;
	OutParams.MaxHealth = MaxHealth;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(Imc_PlayerCharacterInputs);

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

		check(Subsystem);

		Subsystem->AddMappingContext(Imc_PlayerCharacterInputs, 0);
	}

	if (CharacterUI)
	{
		UIWidget = CreateWidget<UUserWidget>(GetWorld(), CharacterUI);

		if (UIWidget)
		{
			UIWidget->AddToViewport();

			// call event to update healthbar using EventUpdateHealthBar event
			if (UFunction* UpdateHealthBarFunction = UIWidget->FindFunction(TEXT("EventUpdateHealthBar")))
			{
				FHealthBarParams Params;
				GetUpdateHealthBarParams(Params);

				UIWidget->ProcessEvent(UpdateHealthBarFunction, &Params);
			}
			
		}
	}
	
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UIWidget)
	{
		UIWidget->RemoveFromParent();
	}


	
	Super::EndPlay(EndPlayReason);
}


void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// setup the enhanced input component
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	// bind input actions
	EnhancedInputComponent->BindAction(Ia_PlayerCharacterMovement, ETriggerEvent::Triggered, this, &APlayerCharacter::CharacterMovement);
	EnhancedInputComponent->BindAction(Ia_PlayerCharacterLookAround, ETriggerEvent::Triggered, this, &APlayerCharacter::CharacterLookAround);
	EnhancedInputComponent->BindAction(Ia_PlayerCharacterJump, ETriggerEvent::Started, this, &APlayerCharacter::CharacterJump);
	EnhancedInputComponent->BindAction(Ia_PlayerCharacterPickupWeapon, ETriggerEvent::Started, this, &APlayerCharacter::CharacterPickupWeapon);
	EnhancedInputComponent->BindAction(Ia_PlayerCharacterShoot, ETriggerEvent::Started, this, &APlayerCharacter::CharacterShoot);
	EnhancedInputComponent->BindAction(Ia_PlayerCharacterCameraZoom, ETriggerEvent::Started, this, &APlayerCharacter::CharacterCameraZoomIn);
	EnhancedInputComponent->BindAction(Ia_PlayerCharacterCameraZoom, ETriggerEvent::Completed, this, &APlayerCharacter::CharacterCameraZoomOut);
}

float APlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	UpdateHealthBarUI();
	
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}


void APlayerCharacter::CharacterMovement(const FInputActionValue& InputActionValue)
{
	FVector2D MovementVector = InputActionValue.Get<FVector2D>();

	if (Controller)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		
		const FVector ForwardDirection = UKismetMathLibrary::GetForwardVector(Rotation);
		const FVector RightDirection = UKismetMathLibrary::GetRightVector(Rotation);
		
		AddMovementInput(ForwardDirection, MovementVector.X);
		AddMovementInput(RightDirection, MovementVector.Y);
	}
}

void APlayerCharacter::CharacterLookAround(const FInputActionValue& InputActionValue)
{
	const FVector2D LookAroundValue = InputActionValue.Get<FVector2D>();

	if (Controller)
	{
		AddControllerPitchInput(LookAroundValue.Y * LookAroundSpeed * -1.f);
		AddControllerYawInput(LookAroundValue.X * LookAroundSpeed);
	}
}

void APlayerCharacter::CharacterJump(const FInputActionValue& InputActionValue)
{
	Jump();
}

void APlayerCharacter::CharacterPickupWeapon(const FInputActionValue& InputActionValue)
{
	if (bCanPickup && Weapon)
	{
		PickupWeapon(Weapon, TEXT("PlayerWeaponSocket"));
		
		bHasRifle = true;

		if (CharacterUI)
		{
			UE_LOG(LogTemp, Warning, TEXT("CharacterUI Set"));
			
			if (UIWidget)
			{
				UE_LOG(LogTemp, Warning, TEXT("UIWidget Set"));
				
				// Call Widget custom Delegate event called Event_UpdateCrosshairVisibility from the UI WBP_BasicUI
				if (UFunction* UpdateCrosshairVisibilityFunction = UIWidget->FindFunction(TEXT("Event_UpdateVisibility")))
				{
					UE_LOG(LogTemp, Warning, TEXT("WidgetUI Event call, updating crosshair visibility"));
					
					UIWidget->ProcessEvent(UpdateCrosshairVisibilityFunction, nullptr);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("WidgetUI Event returned null trying to find: Event_UpdateCrosshairVisibility"));
				}
			}
		}
		
	}
}

void APlayerCharacter::CharacterShoot(const FInputActionValue& InputActionValue)
{
	if (bHasRifle && Weapon)
	{
		Shoot(Weapon);

		// make noise for ai to hear
		MakeNoise(1.f, this, GetActorLocation());
	}
}

void APlayerCharacter::CharacterCameraZoomIn(const FInputActionValue& InputActionValue)
{
	if (bHasRifle)
	{
		SetLookAroundSpeed(ZoomedLookAroundSpeed);
		
		Camera->SetFieldOfView(CameraZoomedFOV);
	}
}

void APlayerCharacter::CharacterCameraZoomOut(const FInputActionValue& InputActionValue)
{
	if (bHasRifle)
	{
		SetLookAroundSpeed(DefaultLookAroundSpeed);
		
		Camera->SetFieldOfView(CameraDefaultFOV);
	}
}

void APlayerCharacter::SetLookAroundSpeed(float NewLookAroundSpeed)
{
	LookAroundSpeed = NewLookAroundSpeed;
}

void APlayerCharacter::UpdateHealthBarUI()
{
	if (UIWidget)
	{
		// call event to update healthbar using EventUpdateHealthBar event
		if (UFunction* UpdateHealthBarFunction = UIWidget->FindFunction(TEXT("EventUpdateHealthBar")))
		{
			FHealthBarParams Params;
			GetUpdateHealthBarParams(Params);

			UIWidget->ProcessEvent(UpdateHealthBarFunction, &Params);
		}
	}
}

void APlayerCharacter::OnDeath()
{
	UpdateHealthBarUI();
}



