// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GunFireProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class AController;

UCLASS(config=Game)
class AGunFireProjectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

public:
	AGunFireProjectile();

    // 발사 시 총알의 변수 초기화를 위한 함수
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void InitializeProjectile(AController* InstigatorController, AActor* DamageCauserActor,
	    float ActualDamage, float Range, float Speed);

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(
	    UPrimitiveComponent* HitComp,
	    AActor* OtherActor,
	    UPrimitiveComponent* OtherComp,
	    FVector NormalImpulse,
	    const FHitResult& Hit
	    );

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

    // 총알의 수명 지정하는 함수
    virtual void SetLifeSpan(float InLifespan) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	float Damage;

	UPROPERTY()
	TObjectPtr<AController> DamageInstigator;

	UPROPERTY()
	TObjectPtr<AActor> DamageCauser;
};
