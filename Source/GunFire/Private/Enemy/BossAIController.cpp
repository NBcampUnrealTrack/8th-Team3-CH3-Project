// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BossAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Enemy/BossEnemy.h"
#include "Kismet/GameplayStatics.h"


ABossAIController::ABossAIController()
    : AEnemyAIController()
{
    MinAttackCooldown = 3.f;
    AttackCooldownDeviation = 2.f;
    TacticChangeTime = 2.f;
}

void ABossAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    AEnemyBase* MyPawn = Cast<AEnemyBase>(GetPawn());
    // 내가 죽었거나 시야 감지가 아니면 무시
    if (!MyPawn || MyPawn->bIsDead() || Stimulus.Type != UAISense::GetSenseID<UAISense_Sight>())
        return;

    if (Actor->ActorHasTag("Player") && Stimulus.WasSuccessfullySensed())
    {
        UBlackboardComponent* BBComp = GetBlackboardComponent();

        // 보스가 현재 대기 상태일 때만 반응해서 '포효(Engage)' 상태로 전환
        if (BBComp && BBComp->GetValueAsInt(BossStateKey) == (int32)EBossState::Idle)
        {
            BBComp->SetValueAsObject(TargetActorKey, Actor);
            BBComp->SetValueAsInt(BossStateKey, (int32)EBossState::Engage);

            // 기존 베이스 클래스에 있는 교전 시작 타이머 가동
            // UpdateCombatTactics를 주기적으로 호출
            StartEngaging(Actor);
        }
    }
}

void ABossAIController::StartEnemyTimer()
{
    StopEngaging();
}

void ABossAIController::UpdateCombatTactics()
{
    UBlackboardComponent* BBComp = GetBlackboardComponent();
    AEnemyBase* MyPawn = Cast<AEnemyBase>(GetPawn());
    AActor* EnemyTarget = Cast<AActor>(BBComp->GetValueAsObject(TargetActorKey));

    if (!BBComp || !MyPawn || MyPawn->bIsDead() || !EnemyTarget)
        return;

    // 공격 중일 땐 모든 판단 정지
    if (bIsAttacking)
        return;

    // 타겟과의 거리 계산
    FVector MyLoc = MyPawn->GetActorLocation();
    FVector TargetLoc = EnemyTarget->GetActorLocation();
    float Distance = FVector::Dist(MyLoc, TargetLoc);
    BBComp->SetValueAsFloat(DistanceKey, Distance);

    if (BBComp->GetValueAsBool(IsCooldownKey))
    {
        int32 Tactic = BBComp->GetValueAsInt(TacticPatternKey);
        FVector DirToTarget = (TargetLoc - MyLoc).GetSafeNormal();
        FVector TacticalPos = MyLoc;

        // 포위
        if (Tactic == 0)
        {
            // 방향벡터와 Z축을 외적하여 타겟을 바라보는 기준 수직으로 이동방향결정
            float CurrentRadius = FVector::Dist(MyLoc, TargetLoc);
            FVector FleeDir = (MyLoc - TargetLoc).GetSafeNormal();
            FVector TangentDir = FVector::CrossProduct(FVector::UpVector, FleeDir);

            // 타겟위치 + (이동방향 * 현재 서로간의 거리(반지름으로사용) * 좌우이동결정)
            FVector NextLoc = TargetLoc + (FleeDir + (TangentDir * EncircleDirection)).GetSafeNormal() * CurrentRadius;
            TacticalPos = NextLoc;
        }
        // 후퇴
        else if (Tactic == 1)
        {
            // 타겟을 바라보는 방향 반대
            FVector FleeDir = -DirToTarget;

            // 현재 위치에서 뒤로 500만큼 떨어진 곳을 목표로 잡음
            TacticalPos = MyLoc + (FleeDir * 500.0f);
        }

        // 블랙보드에 위치 갱신
        BBComp->SetValueAsVector(TacticalLocKey, TacticalPos);
    }
}

void ABossAIController::StartEngaging(AActor* Target)
{
    // 교전시작 시 할거잇다면 여기서 처리
    Super::StartEngaging(Target);
}

void ABossAIController::StopEngaging()
{
    Super::StopEngaging();

    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
        GetWorld()->GetTimerManager().ClearTimer(TacticTimerHandle);
    }
}

void ABossAIController::EndAttackCooldown()
{
    // 공격 쿨타임 종료시 공격 간으상태로 전환한다.
    if (UBlackboardComponent* BBComp = GetBlackboardComponent())
    {
        // 쿨타임상태 해지됨을 알리고 0~2의 패턴중 랜덤하게 사용
        BBComp->SetValueAsBool(IsCooldownKey, false);
        int32 NextPattern = FMath::RandRange(0, 2);
        BBComp->SetValueAsInt(AttackPatternKey, NextPattern);
        GetWorld()->GetTimerManager().ClearTimer(TacticTimerHandle);
    }
}

void ABossAIController::OnHitDamage(APawn* Enemy)
{
    UBlackboardComponent* BBComp = GetBlackboardComponent();
    AEnemyBase* MyPawn = Cast<AEnemyBase>(GetPawn());

    if (!BBComp || !Enemy || !MyPawn || MyPawn->bIsDead())
        return;

    // 타겟정보 등록
    BBComp->SetValueAsObject(TargetActorKey, Enemy);
    BBComp->SetValueAsBool(HasLineOfSightKey, true);

    int32 CurrentBossState = BBComp->GetValueAsInt(BossStateKey);

    // 포효상태면 피격애니메이션x
    if (CurrentBossState == (int32)EBossState::Engage)
    {
        return;
    }

    // 보스가 대기 상태일 때 피격 -> 포효 상태로 전환
    if (CurrentBossState == (int32)EBossState::Idle)
    {
        BBComp->SetValueAsInt(BossStateKey, (int32)EBossState::Engage);
        StartEngaging(Enemy);
        return;
    }

    // 전투 중일 시 피격 리액션 (공격 중일 경우 제외)
    if (!bIsAttacking)
    {
        MyPawn->PlayHitReaction(Enemy);
    }
}

void ABossAIController::SetDead()
{
    StopEngaging();

    if (UBlackboardComponent* BBComp = GetBlackboardComponent())
    {
        BBComp->SetValueAsInt(BossStateKey, (int32)EBossState::Dead);

    }
}

void ABossAIController::ChangeTactic()
{
    if (UBlackboardComponent* BBComp = GetBlackboardComponent())
    {
        // 0: 포위, 1: 후퇴
        float TotalProb = EncircleProbability + FleeProbability;
        float RandomValue = FMath::FRandRange(0.0f, TotalProb);

        // 기본은 0(포위)
        int32 Tactic = 0;

        // 주사위 결과에 따라 변경
        if (RandomValue > EncircleProbability)
        {
            Tactic = 1; // 후퇴
        }

        // 블랙보드에 결정된 전술적용
        BBComp->SetValueAsInt(TacticPatternKey, Tactic);

        // 포위 기동 시 이동 방향은 랜덤
        EncircleDirection = FMath::RandBool() ? 1.0f : -1.0f;
    }
}

void ABossAIController::RoarStart()
{
    ABossEnemy* MyPawn = Cast<ABossEnemy>(GetPawn());
    if (MyPawn)
    {
        // 애니메이션 재생
        MyPawn->PlayRoarAnimation();
        MyPawn->ShowBossHPBar();
    }

}

void ABossAIController::RoarEnd()
{
    // 포효 끝 본격적인 전투 돌입
    if (UBlackboardComponent* BBComp = GetBlackboardComponent())
    {
        BBComp->SetValueAsInt(BossStateKey, (int32)EBossState::Combat);
        // 첫행동할건 공겨긑난것같은 상황에 경계부터.
        OnAttackAnimationFinished();
    }
}

void ABossAIController::OnAttackAnimationFinished()
{
    Super::OnAttackAnimationFinished();

    if (UBlackboardComponent* BBComp = GetBlackboardComponent())
    {
        // 공격 쿨타임 상태
        BBComp->SetValueAsBool(IsCooldownKey, true);

        // 공격 쿨타임 타이머 세팅
        float RandomDeviation = FMath::RandRange(-AttackCooldownDeviation, AttackCooldownDeviation);
        float FinalCooldown = FMath::Max(MinAttackCooldown + RandomDeviation, 0.1f);
        GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, this, &ABossAIController::EndAttackCooldown, FinalCooldown, false);

        // 전술 변경 타이머 시작
        ChangeTactic();
        GetWorld()->GetTimerManager().SetTimer(TacticTimerHandle, this, &ABossAIController::ChangeTactic, TacticChangeTime, true);
    }
}
