// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBase.h"
#include "MinionEnemy.generated.h"

/**
 * 
 */
UCLASS()
class GUNFIRE_API AMinionEnemy : public AEnemyBase
{
	GENERATED_BODY()

protected:
    UPROPERTY()
    TArray<UPrimitiveComponent*> WeaponCollisions;

    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimMontage* AttackMontage;

protected:
    virtual void BeginPlay() override;

public:
    virtual void PlayAttack() override;
    virtual void OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
    virtual void ActivateAttackCollision(FName WeaponTag) override;
    virtual void DeactivateAttackCollision() override;
};
