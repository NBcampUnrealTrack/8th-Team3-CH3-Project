// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBase.h"
#include "BossEnemy.generated.h"

/**
 * 
 */
UCLASS()
class GUNFIRE_API ABossEnemy : public AEnemyBase
{
	GENERATED_BODY()

public:

    virtual void BeginPlay() override;
    // [공격 및 모션]
    // 잡몹의 단발/2연타를 넘어, 페이즈에 따른 광역기, 돌진기 등 다채로운 몽타주를 재생합니다.
    virtual void PlayAttack() override;

    // 보스공격 충돌체 관리
    virtual void ActivateAttackCollision(FName WeaponTag) override;
    virtual void DeactivateAttackCollision() override;

protected:

    // [상태 변화] 
    // 체력 변화
    virtual void OnEnemyHealthChanged(float ActualDamage, AController* EventInstigator) override;

    // 사망
    virtual void Die() override;
    virtual void IMDead() override;


protected:
};
