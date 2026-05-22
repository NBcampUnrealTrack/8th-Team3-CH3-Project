// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyAIController.h"
#include "BossAIController.generated.h"

UENUM(BlueprintType)
enum class EBossState : uint8
{
    Idle        UMETA(DisplayName = "평상시(대기)"),
    Engage      UMETA(DisplayName = "전투 진입(포효)"),   
    Combat      UMETA(DisplayName = "교전 중"),
    Dead        UMETA(DisplayName = "사망")
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

    // 튜토리얼보면서 하다보니...StopEngaging호출용도
    virtual void StartEnemyTimer();

    // 전투관련
    virtual void StartEngaging(AActor* Target);
    virtual void StopEngaging() override;
    void EndAttackCooldown();

    // 피격 반응
    virtual void OnHitDamage(APawn* Enemy) override;

    // 생존 상태
    virtual void SetDead() override;

    // 쿨타임 중 주기적으로 호출될 전술 변경 함수
    void ChangeTactic();

    // 포효상태용
    UFUNCTION(BlueprintCallable, Category = "Boss | Roar")
    void RoarStart();
    UFUNCTION(BlueprintCallable, Category = "Boss | Roar")
    void RoarEnd();



    // 보스전용 블랙보드 키
    const FName BossStateKey = FName("BossState");
    const FName IsCooldownKey = FName("IsCooldown");
    const FName AttackPatternKey = FName("AttackPattern");
    const FName TacticPatternKey = FName("TacticPattern");


    // 쿨타임 (딜레이) 랜덤 범위
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Cooldown")
    float MinAttackCooldown;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Cooldown")
    float AttackCooldownDeviation;

    // 포위, 후퇴행동 딜레이
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Tactic")
    float TacticChangeTime;

    FTimerHandle CooldownTimerHandle;
    FTimerHandle TacticTimerHandle;

    // 공격쿨타임인지 확인용
    bool bIsCooldown = false;
    // 왼쪽으로돌지 오른쪽으로 돌지
    float EncircleDirection = 1.0f;

public:
    // 공격이 끝난 후 처리
    virtual void OnAttackAnimationFinished() override;

    // 공격 상태를 켜고 끌 수 있게 해주는 함수
    UFUNCTION(BlueprintCallable, Category = "Boss | Attack")
    void SetIsAttacking(bool bAttacking) { bIsAttacking = bAttacking; }
};
