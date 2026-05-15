// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/MinionEnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Enemy/EnemyBase.h"

void AMinionEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    // 사망시 진입하면 안됨.
    if (AEnemyBase* MyPawn = Cast<AEnemyBase>(GetPawn()))
    {
        if (MyPawn->bIsDead())
        {
            return;
        }
    }


    // 시야일경우만 진입
    if (Stimulus.Type != UAISense::GetSenseID<UAISense_Sight>())
        return;

    // 타겟이 플레이어인지 확인
    if (Actor->ActorHasTag("Player"))
    {
        if (Stimulus.WasSuccessfullySensed())
        {
            // 전투해제상태대기 해제
            GetWorld()->GetTimerManager().ClearTimer(LoseSightTimerHandle);
            // 시야안에 누군가들어왓으니 전투상태가 되어라
            GetBlackboardComponent()->SetValueAsBool(HasLineOfSightKey, true);
            // 시야안에 들어온 플레이어 등록
            GetBlackboardComponent()->SetValueAsObject(TargetActorKey, Actor);
            StartEngaging(Actor);
        }
        else
        {
            // 시야에 벗어나고 일정시간뒤...
            GetWorld()->GetTimerManager().SetTimer(LoseSightTimerHandle, this, &AMinionEnemyAIController::StartEnemyTimer, LineOfSightTimer, false);
        }
    }
}

void AMinionEnemyAIController::UpdateCombatTactics()
{
    UBlackboardComponent* BBComp = GetBlackboardComponent();
    AEnemyBase* MyPawn = Cast<AEnemyBase>(GetPawn());

    if (!MyPawn || MyPawn->bIsDead())
    {
        return;
    }

    // 공격중일 시 무시
    if (!BBComp || !MyPawn || bIsAttacking)
        return;

    ETacticState CurrentState = (ETacticState)BBComp->GetValueAsInt(TacticStateKey);

    AActor* EnemyTarget = Cast<AActor>(BBComp->GetValueAsObject(TargetActorKey));
    // 대상 타겟이 없다면 여기와선 안됫음
    if (!EnemyTarget)
    {
        StopEngaging();
        return;
    }

    // 기본 정보 수집
    FVector MyLoc = MyPawn->GetActorLocation();
    FVector TargetLoc = EnemyTarget->GetActorLocation();
    float Distance = FVector::Dist(MyLoc, TargetLoc);
    float HPPercent = 1.0f;
    if (MyPawn->GetMaxHP() > 0.f)
    {
        HPPercent = MyPawn->GetHP() / MyPawn->GetMaxHP();
    }

    BBComp->SetValueAsFloat(DistanceKey, Distance);

    //GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Cyan, FString::Printf(TEXT("대상과의 거리 : %.1f"), Distance));

    // 상태 전환 판단
    // TODO : 상태 컴포넌트 추가시 체력비율 넣기
    ETacticState DecidedTactic = DetermineNextTactic(CurrentState, Distance, HPPercent);

    // 포위상태일 경우 어디로 이동할지 결정
    if (DecidedTactic == ETacticState::Encircle)
    {
        bool bIsFirstEntry = (CurrentState != ETacticState::Encircle);
        ProcessEncircleLogic(bIsFirstEntry, MyLoc, TargetLoc);
    }
    // 도주 상태 진입 시 어디로 도주할지 결정(1번만 일어나도록함)
    else if (DecidedTactic == ETacticState::Flee && CurrentState != ETacticState::Flee)
    {
        StartFleeing(MyLoc, TargetLoc);
    }

    // 순서: Dead(0), Groggy(1), Chase(2), Encircle(3), Flee(4), Dash(5), Attack(6)
    BBComp->SetValueAsInt(TacticStateKey, (int32)DecidedTactic);
}

void AMinionEnemyAIController::StopEngaging()
{
    Super::StopEngaging();
    GetWorld()->GetTimerManager().ClearTimer(RetreatTimerHandle);
    GetWorld()->GetTimerManager().ClearTimer(EncircleTimerHandle);
}

ETacticState AMinionEnemyAIController::DetermineNextTactic(ETacticState CurrentState, float Distance, float HPPercent)
{
    // 1. 강제 후퇴 (체력 일정 이하 최초 1회)
    if (!bHasRetreatedDueToLowHP && HPPercent <= LowHealthThreshold)
    {
        bHasRetreatedDueToLowHP = true;
        return ETacticState::Flee;
    }

    // 2. 후퇴중에는 공격진입하지않음
    if (CurrentState == ETacticState::Flee)
        return ETacticState::Flee;

    // 3. 공격 거리 진입
    if (Distance <= AttackDistance)
    {
        GetWorld()->GetTimerManager().ClearTimer(RetreatTimerHandle);
        GetWorld()->GetTimerManager().ClearTimer(EncircleTimerHandle);
        return ETacticState::Attack;
    }

    // 4. 포위중에는 공격외에 다른상태로 변경을 막기
    if (CurrentState == ETacticState::Encircle)
        return ETacticState::Encircle;

    // 5. 전술 구역 진입
    if (Distance <= confDistance)
    {
        // 기존상태가 추격일 경우 전술판단
        if (CurrentState == ETacticState::Chase)
        {
            // 전술을 Dash,Encircle, Flee 랜덤으로 선택
            float RandomValue = FMath::FRand();

            //GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Cyan,
            //    FString::Printf(TEXT("Dice: %.2f | Dash: %.2f | Encircle: %.2f"),
            //        RandomValue, DashProbability, EncircleProbability));

            if (RandomValue <= DashProbability)
                return ETacticState::Dash;
            if (RandomValue <= (DashProbability + EncircleProbability))
                return ETacticState::Encircle;

            return ETacticState::Flee;
        }

        // 돌격 중이었으면 계속 돌격 유지
        if (CurrentState == ETacticState::Dash)
        {
            return ETacticState::Dash;
        }

        // 그외의 예외사항에는 추격
        return ETacticState::Chase;
    }

    // 6. 대시중일시 상대가 대시거리에잇나확인
    if (CurrentState == ETacticState::Dash && Distance <= (confDistance + ExitMargin))
    {
        // 이미 마진 안이면 Dash 유지
        return ETacticState::Dash;
    }
    // 7. 이도저도 아니면 추적
    return ETacticState::Chase;
}

void AMinionEnemyAIController::ProcessEncircleLogic(bool bIsFirstEntry, FVector MyLoc, FVector TargetLoc)
{
    UBlackboardComponent* BBComp = GetBlackboardComponent();

    if (bIsFirstEntry)
    {
        // 왼쪽? 오른쪽? 회전방향 결정
        EncircleDirection = FMath::RandBool() ? 1.0f : -1.0f;

        float RandomEncircleTime = EncircleDuration + FMath::RandRange(0.0f, EncircleRandomDeviation);
        GetWorld()->GetTimerManager().SetTimer(EncircleTimerHandle, this, &AMinionEnemyAIController::StopEncircle, RandomEncircleTime, false);
    }

    float CurrentRadius = FVector::Dist(MyLoc, TargetLoc);
    FVector FleeDir = (MyLoc - TargetLoc).GetSafeNormal();
    FVector TangentDir = FVector::CrossProduct(FVector::UpVector, FleeDir);

    FVector NextLoc = TargetLoc + (FleeDir + TangentDir).GetSafeNormal() * CurrentRadius * EncircleDirection;

    BBComp->SetValueAsVector(TacticalLocKey, NextLoc);

}

void AMinionEnemyAIController::StartFleeing(FVector MyLoc, FVector TargetLoc)
{
    UBlackboardComponent* BBComp = GetBlackboardComponent();

    // 뒤로 도망가는 벡터 계산
    FVector FleeDir = (MyLoc - TargetLoc).GetSafeNormal();
    FVector FleeLoc = MyLoc + (FleeDir * 3000.f);

    BBComp->SetValueAsVector(TacticalLocKey, FleeLoc);

    // 도주 타이머 세팅
    float RandomRetreatTime = RetreatDuration + FMath::RandRange(0.0f, RetreatRandomDeviation);
    GetWorld()->GetTimerManager().SetTimer(RetreatTimerHandle, this, &AMinionEnemyAIController::StopFleeing, RandomRetreatTime, false);
}

void AMinionEnemyAIController::StopEncircle()
{
    UBlackboardComponent* BBComp = GetBlackboardComponent();
    // 혹시 포위상태가 아닌데 호출되었을시 방어용
    if (BBComp && BBComp->GetValueAsInt(TacticStateKey) == (int32)ETacticState::Encircle)
    {
        // 포위 시간이 끝나면 다시 추적으로 변경
        BBComp->SetValueAsInt(TacticStateKey, (int32)ETacticState::Chase);
        UpdateCombatTactics();
    }
}

void AMinionEnemyAIController::StopFleeing()
{
    UBlackboardComponent* BBComp = GetBlackboardComponent();
    // 혹시 도주상태가 아닌데 호출되었을시 방어용
    if (BBComp && BBComp->GetValueAsInt(TacticStateKey) == (int32)ETacticState::Flee)
    {
        // 도주가 끝나면 추격으로 변경
        BBComp->SetValueAsInt(TacticStateKey, (int32)ETacticState::Chase);
        UpdateCombatTactics();
    }
}

void AMinionEnemyAIController::OnAttackAnimationFinished()
{
    bIsAttacking = false;
    OnAttackFinishedDispatcher.Broadcast();
}
