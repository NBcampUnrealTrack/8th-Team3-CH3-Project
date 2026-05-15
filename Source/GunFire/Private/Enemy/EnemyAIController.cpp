// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AEnemyAIController::AEnemyAIController()
    : BTEnemy(nullptr)
{
    AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
    AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdated);


    LineOfSightTimer = 3.0f; // 시야에서 놓친 뒤 추적을 완전히 포기할 때까지의 시간
    UpdateInterval = 0.5f; // 상황 판단 주기
    EncircleProbability = 0.5f; // 포위 확률
    DashProbability = 0.4f; // 돌격 확률
    FleeProbability = 0.1f; // 후퇴 확률
    LowHealthThreshold = 0.3f; // 후퇴를 결심하는 체력 비율
    RetreatDuration = 3.0f; // 후퇴 유지 시간
    EncircleRadius = 600.0f; // 포위 반경
    AttackDistance = 200.0f; // 공격상태변경 거리
    confDistance = 600.0f; // 추적이후 다른상태 변경이될 거리
    ExitMargin = 600.f;

    // 블랙보드 키 이름
    TargetActorKey = FName("EnemyActor");
    DistanceKey = FName("DistanceToTarget");
    TacticStateKey = FName("TacticState");
    TacticalLocKey = FName("TacticalLocation");
    HasLineOfSightKey = FName("HasLineOfSight");

    //TraceSpeed = 400.f;
    //DashSpeed = 800.f;
    //FleeSpeed = 200.f;
}

void AEnemyAIController::OnAttackAnimationFinished()
{
    bIsAttacking = false;

    OnAttackFinishedDispatcher.Broadcast();
}

void AEnemyAIController::ForceResetAttack()
{
    bIsAttacking = false;
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    // 에디터에서 BT를 제대로 넣었는지 확인(블랙보드랑 연동은 필수)
    if (BTEnemy != nullptr)
    {
        // 이 함수가 내부적으로 블랙보드까지 세팅하고 트리를 가동
        RunBehaviorTree(BTEnemy);
    }
}

void AEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
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
            GetWorld()->GetTimerManager().SetTimer(LoseSightTimerHandle, this, &AEnemyAIController::StartEnemyTimer, LineOfSightTimer, false);
        }
    }
}

void AEnemyAIController::StartEnemyTimer()
{
    StopEngaging();
}

void AEnemyAIController::StartEngaging(AActor* Target)
{
    // 교전 시작: UpdateInterval(0.5초) 마다 전술 업데이트 실행
    // 실행하자마자 한번은 실행되도록함
    if (!GetWorld()->GetTimerManager().IsTimerActive(CombatUpdateTimerHandle))
    {
        UpdateCombatTactics();
        GetWorld()->GetTimerManager().SetTimer(CombatUpdateTimerHandle, this, &AEnemyAIController::UpdateCombatTactics, UpdateInterval, true);
    }
}

void AEnemyAIController::StopEngaging()
{
    // 전투상태가 아님을 알림
    GetBlackboardComponent()->SetValueAsBool(HasLineOfSightKey, false);
    // 타겟 비움
    GetBlackboardComponent()->ClearValue(TargetActorKey);

    // 내부 전술 변수 및 타이머 초기화
    GetWorld()->GetTimerManager().ClearTimer(CombatUpdateTimerHandle);
}

void AEnemyAIController::SetGroggy()
{
}

void AEnemyAIController::SetDead()
{
}

void AEnemyAIController::UpdateCombatTactics()
{
    UBlackboardComponent* BBComp = GetBlackboardComponent();
    if (!BBComp || !GetPawn())
        return;

    // 사망상태와 그로기시는 무시
    //if (MyPawn->IsDead() || MyPawn->IsGroggy())
    //{
    //    bIsAttacking = false; // 공격 잠금 강제 해제
    //    BBComp->SetValueAsInt(TacticStateKey, MyPawn->IsDead() ? 0 : 1);
    //    return;
    //}

    // 공격 중일시 무시
    if (bIsAttacking)
        return;

    // 블랙보드에서 타겟 직접 획득
    AActor* EnemyTarget = Cast<AActor>(BBComp->GetValueAsObject(TargetActorKey));
    if (!EnemyTarget)
    {
        StopEngaging();
        return;
    }

    // 대상과의 거리 습득
    float Distance = FVector::Dist(GetPawn()->GetActorLocation(), EnemyTarget->GetActorLocation());
    BBComp->SetValueAsFloat(DistanceKey, Distance);
    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("TargetDistance : %f"), Distance));

    // 블랙보드에 저장된 상태 가져옴
    int32 CurrentState = BBComp->GetValueAsInt(TacticStateKey);
    ETacticState DecidedTactic = (ETacticState)CurrentState;

    // 전술 변경
    // 공격 거리(AttackDistance) 안인가?
    if (Distance <= AttackDistance)
    {
        DecidedTactic = ETacticState::Attack; // Attack (Enum 순서: 0:Dead, 1:Groggy, 2:Chase, 3:Encircle, 4:Flee, 5:Dash, 6:Attack)
        //bIsAttacking = true;
    }
    else
    {
        // [전술] 이미 전술 상태인 경우
        if (CurrentState == (int32)ETacticState::Dash)
        {
            // 전술 진입 거리(600) + 마진(200) = 800을 넘어가야만 추적으로 바꿈
            if (Distance > (confDistance + ExitMargin))
            {
                DecidedTactic = ETacticState::Chase;
            }
        }
        // 추적 중인 경우 전술 진입 시도
        else if (Distance <= confDistance)
        {
            DecidedTactic = ETacticState::Dash;
        }
        // [추적] 상대와의 거리가 멀다
        else
        {
            DecidedTactic = ETacticState::Chase;
        }
    }

    // 순서: Dead(0), Groggy(1), Chase(2), Encircle(3), Flee(4), Dash(5), Attack(6)
    BBComp->SetValueAsInt(TacticStateKey, (int32)DecidedTactic);
}
