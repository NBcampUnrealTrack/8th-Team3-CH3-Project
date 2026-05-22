// Copyright Epic Games, Inc. All Rights Reserved.

#include "GunFireProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

AGunFireProjectile::AGunFireProjectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AGunFireProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

	Damage = 0.f;
	DamageInstigator = nullptr;
	DamageCauser = nullptr;
}

void AGunFireProjectile::InitializeProjectile(AController* InstigatorController, AActor* DamageCauserActor,
        float ActualDamage, float Range, float Speed)
{
	Damage = ActualDamage;
	DamageInstigator = InstigatorController;
	DamageCauser = DamageCauserActor;

    // 속도 설정
    // 수명 = 사거리 / 속도
    ProjectileMovement->InitialSpeed = Speed;
    ProjectileMovement->MaxSpeed = Speed;
    ProjectileMovement->Velocity = GetActorForwardVector() * Speed;
    SetLifeSpan(Range / Speed);

    // 총알을 발사한 캐릭터와 충돌하는것 방지
	if (IsValid(CollisionComp) && IsValid(DamageCauser))
	{
		CollisionComp->IgnoreActorWhenMoving(DamageCauser, true);
	}
}

void AGunFireProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!IsValid(OtherActor) || OtherActor == this || OtherActor == DamageCauser.Get()) return;

	if (Damage > 0.f)
	{
		UGameplayStatics::ApplyDamage(
			OtherActor,
			Damage,
			DamageInstigator.Get(),
			DamageCauser.Get(),
			UDamageType::StaticClass()
		);
	}

	Destroy();
}

void AGunFireProjectile::SetLifeSpan(float InLifespan)
{
    Super::SetLifeSpan(InLifespan);

    InitialLifeSpan = InLifespan;
}
