// Domain Shooter By Brian


#include "WeaponProjectiles/Projectile.h"

#include "Characters/HumanCharacterBase.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"


AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	RootComponent = SphereCollision;
	SphereCollision->SetSphereRadius(10.f);
	SphereCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(GetRootComponent());
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;

	InitialLifeSpan = 5.f;

	
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	SphereCollision->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
}

// for overlap collisions
void AProjectile::OnComponentOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UWorld* World = GetWorld();
	if (World && ImpactParticle && ImpactSound)
	{
		// particle effect
		UGameplayStatics::SpawnEmitterAtLocation(World, ImpactParticle, GetActorTransform());

		// play sound
		UGameplayStatics::PlaySoundAtLocation(World, ImpactSound, GetActorLocation());
	}

	Destroy();
}

// for blocked collisions
void AProjectile::OnHit(UPrimitiveComponent* HitComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UWorld* World = GetWorld();
	if (World && ImpactParticle && ImpactSound)
	{
		// particle effect
		UGameplayStatics::SpawnEmitterAtLocation(World, ImpactParticle, GetActorTransform());

		// play sound
		UGameplayStatics::PlaySoundAtLocation(World, ImpactSound, GetActorLocation());
	}

	if (OtherActor->IsA<AHumanCharacterBase>())
	{
		UE_LOG(LogTemp, Warning, TEXT("%.2f damage to %s"), BaseDamage, *OtherActor->GetName());
		
		UGameplayStatics::ApplyDamage(OtherActor, BaseDamage, GetInstigatorController(), this, UDamageType::StaticClass());
	}

	Destroy();
}
