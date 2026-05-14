#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MeleeCombatComponent.generated.h"

class AEnemyBase;
class ACharacter;
class AMeleeWeaponBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAttackFinishedSignature);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUNFIRE_API UMeleeCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMeleeCombatComponent();

    bool CanStartAttack(AMeleeWeaponBase* MeleeWeapon);

    bool TryLightAttack(AMeleeWeaponBase* MeleeWeapon, float AttackPower);
    bool TryHeavyAttack(AMeleeWeaponBase* MeleeWeapon, float AttackPower);

    UFUNCTION(BlueprintCallable, Category = "Combat|Melee")
    void FinishAttack();

    UFUNCTION(BlueprintCallable, Category = "Combat|Melee")
    void BeginAttackTrace();

    UFUNCTION(BlueprintCallable, Category = "Combat|Melee")
    void EndAttackTrace();

public:
    UPROPERTY(BlueprintAssignable, Category = "Combat|Melee")
    FAttackFinishedSignature OnAttackFinished;

protected:
	virtual void BeginPlay() override;

protected:
    // 컴포넌트 소유자
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Melee")
    TObjectPtr<ACharacter> OwnerCharacter;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Melee")
    int32 CurrentComboIndex;

    // 테스트를 위한 변수
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Melee")
    bool bFinishedAttackForTest;

private:
    void TraceAttack();

private:
    FTimerHandle AttackFinishTimerHandle;
    FTimerHandle AttackTraceTimerHandle;

    // 트레이스에 사용할 무기
    UPROPERTY()
    TWeakObjectPtr<AMeleeWeaponBase> CurrentMeleeWeapon;

    // 맞은 적 Set에 담아서 중복처리 못하게
    UPROPERTY()
    TSet<TObjectPtr<AEnemyBase>> HitActors;

    // 현재 데미지
    float CurrentPower;

    // 트레이스 간격
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Melee|Trace", meta = (AllowPrivateAccess = "true"))
    float AttackTraceInterval;
};
