// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

// 델리게이트에서 EnemyBase 포인터를 넘기기 위한 전방선언
class AEnemyBase;

// Delegate/Event 방식으로 몬스터 사망시 Room 에 알리기 위함
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDied, AEnemyBase*, DeadEnemy);

class UStatComponent;

UCLASS()
class GUNFIRE_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyBase();
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStatComponent* StatComponent;

    // 델리게이트 타입의 변수 생성
    UPROPERTY(BlueprintAssignable, Category = "Enemy")
    FOnEnemyDied OnEnemyDead;

    // 사망 애니메이션 몽타주
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* DeathMontage;

    // 피격 시 재생할 몽타주
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* HitMontage;

    // 삭제를 위한 타이머
    FTimerHandle DeathTimerHandle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Combat")
    bool bIsPatrol = true; // 패트롤 여부

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Combat")
    bool bCanBeStunned = true; // 피격 시 경직 허용 여부

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Combat")
    float HitStunDuration = 1.5f; // 경직 지속 시간

    // 피격 재생
    virtual void PlayHitReaction(APawn* Attacker);

    UFUNCTION()
    virtual void OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual void ActivateAttackCollision(FName WeaponTag);
    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual void DeactivateAttackCollision();
    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual void OnDeathAnimationFinished();

    UFUNCTION()
    virtual void Die();
    UFUNCTION()
    void ExecuteDestroy();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackSpeedRate;

    UPROPERTY(VisibleAnywhere, Category = "Stats")
    bool bDead;

    // 디버그 테스트용 스위치
    UPROPERTY(EditAnywhere, Category = "Debug")
    bool bIsDebugTestTarget = false;

    UFUNCTION()
    virtual void OnEnemyDeath(AController* InstigatorController);

    UFUNCTION()
    virtual void OnEnemyHealthChanged(float ActualDamage, AController* EventInstigator);

public:
    // 이동속도 변경
    void SetWalkSpeed(float _speed);

    // 사망상태
    virtual void IMDead();

    //그로기 상태
    virtual void IMGrogi();

    // 공격애니메이션 재생
    // 부모에 구현된 코드는 예시코드임
    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual void PlayAttack();

    //// 피해 처리
    //UFUNCTION(BlueprintCallable, Category = "Health")
    //virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    float GetHP() const;
    float GetMaxHP() const;
    float GetAttackDamage() const;
    bool bIsDead() const;
    bool GetIsPatrol() const { return bIsPatrol; }
    bool GetCanBeStunned() const { return bCanBeStunned; }
    float GetHitStunDuration() const { return HitStunDuration; }
};
