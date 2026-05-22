// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Enemy/EnemyBase.h"
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
    RetreatDuration = 2.0f; // 후퇴 유지 시간
    RetreatRandomDeviation = 1.f;
    EncircleRadius = 600.0f; // 포위 반경
    AttackDistance = 200.0f; // 공격상태변경 거리
    confDistance = 600.0f; // 추적이후 다른상태 변경이될 거리
    ExitMargin = 600.f;
    EncircleDuration = 3.f;
    EncircleRandomDeviation = 2.f;

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

    // 내가 빙의한 몸통이 EnemyBase인지 확인
    AEnemyBase* MyEnemy = Cast<AEnemyBase>(InPawn);

    if (BTEnemy != nullptr && MyEnemy)
    {
        RunBehaviorTree(BTEnemy);
        // 패트롤 값 블랙보드에 전달
        GetBlackboardComponent()->SetValueAsBool(IsPatrolKey, MyEnemy->GetIsPatrol());
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
    // 실행하자마자 한번은 실행되도록함. 기준은 CombatUpdateTimerHandle사용중인가
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
    UBlackboardComponent* BBComp = GetBlackboardComponent();
    if (BBComp)
    {
        // 사망상태로 변경
        BBComp->SetValueAsInt(TacticStateKey, 0);

        // 타이머, 타겟 정보 비우기
        StopEngaging();
    }
}

void AEnemyAIController::OnHitDamage(APawn* Enemy)
{
    UBlackboardComponent* BBComp = GetBlackboardComponent();
    AEnemyBase* MyPawn = Cast<AEnemyBase>(GetPawn());
    if (!BBComp || !Enemy || !MyPawn)
        return;

    // 공격 중이 아닐 때 경직 처리
    if (MyPawn->GetCanBeStunned() && !bIsAttacking)
    {
        // 상태를 Groggy로 변경
        BBComp->SetValueAsInt(TacticStateKey, (int32)ETacticState::Groggy);

        // 설정된 경직 시간만큼 타이머 작동
        GetWorld()->GetTimerManager().ClearTimer(HitStunTimerHandle);
        GetWorld()->GetTimerManager().SetTimer(HitStunTimerHandle, this, &AEnemyAIController::EndHitStun, MyPawn->GetHitStunDuration(), false);

        // 피격 몽타주 재생
        MyPawn->PlayHitReaction(Enemy);
    }

    // 블랙보드에 타겟 등록 및 시야 확보 처리
    BBComp->SetValueAsObject(TargetActorKey, Enemy);
    BBComp->SetValueAsBool(HasLineOfSightKey, true);

    // 경직을 줄거고 HasLineOfSightKey가 true이니 자동으로 그쪽을 볼거임
    //// 맞자마자 타겟으로 방향전환연산
    //FVector Direction = Enemy->GetActorLocation() - MyPawn->GetActorLocation();
    //
    //// 방향 벡터를 변환
    //FRotator TargetRot = Direction.Rotation();
    //
    //// 회전값(어차피 z축 회전이면될것)
    //FRotator NewRotation = FRotator(0.f, TargetRot.Yaw, 0.f);
    //MyPawn->SetActorRotation(NewRotation);

    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("테스트 타겟 피격! 동료들을 부릅니다!"));


    // 반경 내 동료에게 알람
    AlertAlly(Enemy);
}

void AEnemyAIController::AlertAlly(APawn* Target)
{
    if (!Target || !GetPawn())
        return;

    // 원형 트레이스를 통해 모든 액터 검출
    APawn* MyPawn = GetPawn();

    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(MyPawn);
    TArray<FHitResult> OutHits;
    const float Radius = 2000.f;

    // 디버그용
    // 구형태로 검사진행
    bool bHit = UKismetSystemLibrary::SphereTraceMulti(
        GetWorld(),
        MyPawn->GetActorLocation(),
        MyPawn->GetActorLocation(),
        Radius,
        UEngineTypes::ConvertToTraceType(TraceChannel),
        false,
        ActorsToIgnore,
        EDrawDebugTrace::ForDuration,
        OutHits,
        true,
        FLinearColor::Green,
        FLinearColor::Red,
        2.0f
    );
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("주변 아군의 수 : %d"), OutHits.Num()));

    if (bHit)
    {
        // 아군에게 신호전달
        for (FHitResult& Hit : OutHits)
        {
            AEnemyBase* Ally = Cast<AEnemyBase>(Hit.GetActor());

            // 죽은 아군은 호출할 필요없음
            if (Ally && !Ally->bIsDead())
            {
                if (AEnemyAIController* AllyAIC = Cast<AEnemyAIController>(Ally->GetController()))
                {
                    AllyAIC->ReceiveAlert(Target);
                }
            }
        }
    }


    // 실제 완성시 사용
    //GetWorld()->SphereTrace
}

void AEnemyAIController::ReceiveAlert(APawn* Target)
{
    UBlackboardComponent* BBComp = GetBlackboardComponent();
    if (!BBComp || !Target) return;

    // 이미 다른 플레이어나 적과 싸우는 중이라면 무시
    if (BBComp->GetValueAsObject(TargetActorKey) != nullptr)
        return;

    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("아군이 신호를 전달받앗습니다!"));

    APawn* MyPawn = GetPawn();
    // 대상을 타겟으로 방향전환
    FVector Direction = Target->GetActorLocation() - MyPawn->GetActorLocation();
    FRotator TargetRot = Direction.Rotation();
    FRotator NewRotation = FRotator(0.f, TargetRot.Yaw, 0.f);
    MyPawn->SetActorRotation(NewRotation);

    // 교전상태로 돌입
    BBComp->SetValueAsObject(TargetActorKey, Target);
    BBComp->SetValueAsBool(HasLineOfSightKey, true);

    StartEngaging(Target);
}

void AEnemyAIController::EndHitStun()
{
    UBlackboardComponent* BBComp = GetBlackboardComponent();
    if (BBComp)
    {
        // 전술 업데이트 실행
        BBComp->SetValueAsInt(TacticStateKey, (int32)ETacticState::Chase);
        UpdateCombatTactics();
    }
}

void AEnemyAIController::UpdateCombatTactics()
{
    UBlackboardComponent* BBComp = GetBlackboardComponent();
    if (!BBComp || !GetPawn())
        return;

    // 경직중에는 행동 무시
    ETacticState CurrentState = (ETacticState)BBComp->GetValueAsInt(TacticStateKey);
    if (CurrentState == ETacticState::Groggy)
    {
        return;
    }

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
    //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("TargetDistance : %f"), Distance));

    // 블랙보드에 저장된 상태 가져옴
    ETacticState DecidedTactic = CurrentState;

    // 전술 변경
    // 공격 거리(AttackDistance) 안인가?
    if (Distance <= AttackDistance)
    {
        DecidedTactic = ETacticState::Attack; // Attack (Enum 순서: 0:Dead, 1:Groggy, 2:Chase, 3:Encircle, 4:Flee, 5:Dash, 6:Attack)
        bIsAttacking = true;
    }
    else
    {
        // [전술] 이미 전술 상태인 경우
        if (CurrentState == ETacticState::Dash)
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
