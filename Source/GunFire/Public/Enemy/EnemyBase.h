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

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStatComponent* StatComponent;

    // 델리게이트 타입의 변수 생성
    UPROPERTY(BlueprintAssignable, Category = "Enemy")
    FOnEnemyDied OnEnemyDead;

    // 사망 애니메이션 몽타주
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* DeathMontage;

    // 삭제를 위한 타이머
    FTimerHandle DeathTimerHandle;


    UFUNCTION()
    virtual void OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual void ActivateAttackCollision(FName WeaponTag);
    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual void DeactivateAttackCollision();
    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual void OnDeathAnimationFinished();

    UFUNCTION()
    void Die();
    UFUNCTION()
    void ExecuteDestroy();

protected:

    //UPROPERTY(EditAnywhere, Category = "Stats")
    //float HP;
    //UPROPERTY(EditAnywhere, Category = "Stats")
    //float MaxHP;
    //
    //UPROPERTY(EditAnywhere, Category = "Stats")
    //float Defance;
    //
    //UPROPERTY(EditAnywhere, Category = "Stats")
    //float AttackDamage;
    //
    UPROPERTY(VisibleAnywhere, Category = "Stats")
    bool bDead;

	virtual void BeginPlay() override;

    UFUNCTION()
    void OnEnemyDeath(AController* InstigatorController);

    UFUNCTION()
    void OnEnemyHealthChanged(float CurrentHealth, float MaxHealth);

public:
    // 이동속도 변경
    void SetWalkSpeed(float _speed);

    // 사망상태
    virtual void IMDead();

    //그로기 상태
    virtual void IMGrogi();

    // 공격애니메이션 재생
    // 부모에 구현된 코드는 예시코드임
    UFUNCTION(BlueprintCallable, Category = "AI|Combat")
    virtual void PlayAttack();

    //// 피해 처리
    //UFUNCTION(BlueprintCallable, Category = "Health")
    //virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    float GetHP() const;
    float GetMaxHP() const;
    float GetAttackDamage() const;
    bool bIsDead() const;
};
