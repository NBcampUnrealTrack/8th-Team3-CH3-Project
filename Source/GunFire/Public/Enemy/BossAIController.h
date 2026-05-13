// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyAIController.h"
#include "BossAIController.generated.h"

UENUM(BlueprintType)
enum class EBossState : uint8
{
    Idle        UMETA(DisplayName = "평상시(대기)"),      // 0: 플레이어 발견 전
    Engage      UMETA(DisplayName = "전투 진입(포효)"),   // 1: 플레이어 발견 직후 포효 연출
    Combat      UMETA(DisplayName = "교전 중"),           // 2: 본격적인 패턴 전투 진입
    Dead        UMETA(DisplayName = "사망")               // 3: 최우선 상태
};

UCLASS()
class GUNFIRE_API ABossAIController : public AEnemyAIController
{
	GENERATED_BODY()
public:
    ABossAIController();

protected:
    // 인식
    virtual void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus) override;

    // 전술 업데이트
    virtual void UpdateCombatTactics() override;

    // 전투 해제
    virtual void StopEngaging() override;

    // 피격 반응
    virtual void OnHitDamage(APawn* Enemy) override;

    // 생존 상태
    virtual void SetDead() override;

public:
    // 공격이 끝난 후 처리
    virtual void OnAttackAnimationFinished() override;
};
