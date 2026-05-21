// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBase.h"
#include "BossEnemy.generated.h"

class UUserWidget;
class ABossHPUI;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossDamage);

UCLASS()
class GUNFIRE_API ABossEnemy : public AEnemyBase
{
	GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable, Category = "Boss")
    FOnBossDamage OnEnemyHit;

    virtual void BeginPlay() override;

    virtual void OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

    // 보스공격 충돌체 관리
    virtual void ActivateAttackCollision(FName WeaponTag) override;
    virtual void DeactivateAttackCollision() override;

    // 애니메이션 재생함수들
    virtual void PlayRoarAnimation();

    // 보스 전용 공격 함수
    void PlayBossAttack(int32 PatternIndex);

    // 광역기 판정 함수
    UFUNCTION(BlueprintCallable, Category = "Boss | Combat")
    void ExecuteStomp();

    // HPUI관련 함수
    virtual void ShowBossHPBar();
    virtual void HideBossHPBar();
    virtual void UpdateBossHPBar();

protected:

    // [상태 변화] 
    // 체력 변화
    virtual void OnEnemyHealthChanged(float ActualDamage, AController* EventInstigator) override;

    // 공격
    virtual void PlayAttack() override;

    // [공격 오버랩용]
    void HandleLightAttackHit(ACharacter* HitPlayer);
    void HandleHeavyAttackHit(ACharacter* HitPlayer);
    void HandleStompDirectHit(ACharacter* HitPlayer);

    // 공격요 충돌체
    UPROPERTY()
    TArray<UPrimitiveComponent*> WeaponCollisions;

    // 포효
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Action")
    UAnimMontage* RoarMontage;

    // 공격
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Action")
    UAnimMontage* AttackMontage;

    // 공격 적중시 사운드
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    class USoundBase* HitSound;

    // 현재 실행 중인 공격 패턴 번호 저장용
    int32 CurrentAttackPattern = 0;

    // 약공격 배율
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Combat")
    float LightAttackDamageMultiplier = 0.8f;

    // 강공격 데미지/넉백힘 배율 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Combat")
    float HeavyAttackDamageMultiplier = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Combat")
    float HeavyKnockbackForce = 1000.0f;

    // 광역기 범위/넉백힘/ 데미지 배율
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Combat")
    float StompRadius = 1500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Combat")
    float StompKnockupForce = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Combat")
    float StompAttackDamageMultiplier = 2.0f;

    // 사망
    virtual void Die() override;
    virtual void IMDead() override;

    // 스톰프 공격 전 경고용 데칼
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Decal")
    TSoftClassPtr<AActor> WarningDecalSoftClass;
    // 스톰프 타격 후 바닥이 깨진 느낌을 주는 데칼
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | Decal")
    TSoftClassPtr<AActor> ImpactDecalSoftClass;

    // 비동기 로드가 완료된 사용할 클래스
    UPROPERTY()
    UClass* LoadedWarningDecalClass;

    UPROPERTY()
    UClass* LoadedImpactDecalClass;
    // 데칼 로드시 호출
    void OnDecalLoadCompleted();

    // hpui용
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss | UI")
    TSubclassOf<ABossHPUI> BossHPUIClass;

    UPROPERTY()
    TObjectPtr<ABossHPUI> BossHPUIInstance;

    void SpawnStompWarningDecal();
    void SpawnStompImpactDecal();


protected:
};
