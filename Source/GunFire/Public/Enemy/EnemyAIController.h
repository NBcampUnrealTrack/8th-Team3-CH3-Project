// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class UBehaviorTree;
class UBlackboardData;
class UAIPerceptionComponent;

UENUM(BlueprintType)
enum class ETacticState : uint8
{
    Dead        UMETA(DisplayName = "사망 (Dead)"),
    Groggy      UMETA(DisplayName = "스턴 (Groggy)"),
    Chase       UMETA(DisplayName = "추적 (Chase)"),
    Encircle    UMETA(DisplayName = "포위 (Encircle)"),
    Flee        UMETA(DisplayName = "후퇴 (Flee)"),
    Dash        UMETA(DisplayName = "돌진 (Dash)"),
    Attack      UMETA(DisplayName = "공격 (Attack)")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackFinishedDelegate);

UCLASS()
class GUNFIRE_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
public:
    AEnemyAIController();

    // 애니메이션 노티파이에서 호출할 공격 종료 알림 함수
    // 상속 받아서 쓸 것
    //UFUNCTION(BlueprintCallable, Category = "AI|Combat")
    //virtual void ActivateAttackCollision() {}

    // 공격 애니메이션 중 특정 프레임에서 충돌체를 다시 비활성화하기 위한 함수
    // 상속 받아서 쓸 것
    //UFUNCTION(BlueprintCallable, Category = "AI|Combat")
    //virtual void DeactivateAttackCollision() {}

    // 공격 애니메이션 자체가 완전히 끝났음을 알리는 함수
    // 상속 받아서 쓸 것
    UFUNCTION(BlueprintCallable, Category = "AI|Combat")
    virtual void OnAttackAnimationFinished();

    // 이벤트 디스패쳐
    // 공격이 끝낫음을 알림
    UPROPERTY(BlueprintAssignable, Category = "AI|Combat")
    FOnAttackFinishedDelegate OnAttackFinishedDispatcher;

    // 공격 강제 종료
    void ForceResetAttack();

    // 교전시작과 종료
    void StartEngaging(AActor* Target);
    void StopEngaging();

    // 그로기나 사망
    void SetGroggy();
    void SetDead();
protected:
    // 적 ai빙의시
    virtual void OnPossess(APawn* InPawn) override;

    // 비헤이비어 트리
    // 블랙보드는 블루프린트에서 연결시킴
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    UBehaviorTree*      BTEnemy;

    // ai퍼셉션
    // ai의 여러자극을 통제함(시야, 소리등)
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
    UAIPerceptionComponent* AIPerceptionComp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Perception")
    float LineOfSightTimer; // 시야에서 놓친 뒤 추적을 완전히 포기할 때까지의 시간

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    float UpdateInterval; // 상황 판단 주기

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    float EncircleProbability; // 포위 확률

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    float DashProbability; // 돌격 확률

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    float FleeProbability; // 후퇴 확률

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Config")
    float LowHealthThreshold; // 후퇴를 결심하는 체력 비율

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    float RetreatDuration; // 후퇴 유지 시간

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    float EncircleRadius; // 포위 반경

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    float AttackDistance; // 공격상태변경 거리

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    float confDistance; // 전술 진입 거리

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    float ExitMargin; // 전술 유지 마진

    //UPROPERTY(EditDefaultsOnly, Category = "AI|Combat")
    //float TraceSpeed; // 추적 속도
    //UPROPERTY(EditDefaultsOnly, Category = "AI|Combat")
    //float DashSpeed; // 돌격 속도
    //UPROPERTY(EditDefaultsOnly, Category = "AI|Combat")
    //float FleeSpeed; // 포위 속도

    // 공격 중일 때 UpdateCombatTactics가 상태를 바꾸지 못하게 막는 변수
    bool bIsAttacking = false;

    // 블랙보드 키 이름
    FName TargetActorKey = FName("EnemyActor");
    FName DistanceKey = FName("DistanceToTarget");
    FName TacticStateKey = FName("TacticState");
    FName TacticalLocKey = FName("TacticalLocation");
    FName HasLineOfSightKey = FName("HasLineOfSight");

    // 타이머
    FTimerHandle CombatUpdateTimerHandle; // 전투생각을 위한 타이머
    FTimerHandle LoseSightTimerHandle;  // 시야에 벗어날 시 순찰로 복귀한느 시간

    // 퍼셉션 이벤트 감지
    UFUNCTION()
    virtual void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

    // 타겟을 놓치고 일정 시간 뒤 호출될 이벤트
    UFUNCTION()
    void StartEnemyTimer();

    // 핵심 전술 판단 로직
    // 전투시 0.5초마다 실행됨. Combat노드에서 들어와야 실행되도록 해야함(HasLineOfSightKey가 true)
    UFUNCTION(BlueprintCallable, Category = "AI|Combat")
    virtual void UpdateCombatTactics();

};
