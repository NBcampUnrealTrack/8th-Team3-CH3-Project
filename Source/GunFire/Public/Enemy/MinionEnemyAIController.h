// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyAIController.h"
#include "MinionEnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class GUNFIRE_API AMinionEnemyAIController : public AEnemyAIController
{
	GENERATED_BODY()

protected:
    virtual void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus) override;
    virtual void UpdateCombatTactics() override;

    // 어떤 전술을 사용할지 결정
    ETacticState DetermineNextTactic(ETacticState CurrentState, float Distance, float HPPercent);

    // 포위 좌표를 계산
    void ProcessEncircleLogic(bool bIsFirstEntry, FVector MyLoc, FVector TargetLoc);

    // 도주 좌표 및 타이머를 설정
    void StartFleeing(FVector MyLoc, FVector TargetLoc);

    // 포위, 도주끝나면 할 행동
    void StopEncircle();
    void StopFleeing();

    // 회전할 방향용
    float EncircleDirection = 1.0f;
    // 
    bool bHasRetreatedDueToLowHP = false;

    // 후퇴 유지용 타이머
    FTimerHandle EncircleTimerHandle;
    FTimerHandle RetreatTimerHandle;

public:
    virtual void OnAttackAnimationFinished() override;
};
