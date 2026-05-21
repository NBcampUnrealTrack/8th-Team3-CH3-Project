#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapon/MeleeWeaponBase.h"
#include "CombatComponent.generated.h"


class AMeleeWeaponBase;
class UStatComponent;
class UWeaponComponent;
class URangedCombatComponent;
class UMeleeCombatComponent;

// 전투 행동 상태
UENUM(BlueprintType)
enum class ECombatActionState : uint8
{
    None            UMETA(DisplayName = "없음"),
    Attacking       UMETA(DisplayName = "공격"),
    Reloading       UMETA(DisplayName = "재장전"),
    Dodging         UMETA(DisplayName = "회피"),
    Stunned         UMETA(DisplayName = "피격 경직"),
    UsingSkill      UMETA(DisplayName = "스킬 사용"),
    Interacting     UMETA(DisplayName = "상호작용"),
    Dead            UMETA(DisplayName = "사망")
};

// 전투 행동 상태가 전환되었음을 알리는 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FCombatActionStateChangedSignature, ECombatActionState, PreviousState, ECombatActionState, NewState);

// 어떤 행동을 실행할지를 결정하는 클래스, 전투 세부 컴포넌트들을 관리함
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUNFIRE_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();

    /* 전투 입력으로 인한 행동들 */

    // 근접 무기 약공격 시도
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void TryLightAttack();

    // 근접 무기 강공격 시도
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void TryHeavyAttack();

    // 원거리 공격 시도
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void TryRangedAttack();

    // 재장전 시도
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void TryReload();

    // 회피 시도
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void TryDodge();


    /* 외부 사건으로 인한 상태 변화, ex) 사망, 피격, 상호작용 */

    // 행동 상태 전환 시도
    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool TrySetActionState(ECombatActionState NewState);

    // 행동 상태 날리기
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void ClearActionState(ECombatActionState ExpectedState);

    // 강제 상태 전환, ex) 사망, 피격
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void ForceSetActionState(ECombatActionState NewState);

    // 피격 경직 상태로 전환
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void GetStunned();

    // 상호작용 상태로 전환
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void StartInteracting();


    /* Getter, Setter, 확인 함수 */

    // 전투 행동을 시작해도 되는 상태인지 확인
    UFUNCTION(BlueprintPure, Category = "Combat")
    bool CanStartCombatAction() const;

    // 해당 행동으로 전환이 되는지 확인
    UFUNCTION(BlueprintPure, Category = "Combat")
    bool CanChangeActionState(ECombatActionState NewState) const;

    // 현재 행동 상태 Getter
    UFUNCTION(BlueprintPure, Category = "Combat")
    ECombatActionState GetCurrentActionState() const;

    // 전투 가능한지 확인
    UFUNCTION(BlueprintPure, Category = "Combat")
    bool IsCombatEnabled() const;

    // 전투 가능한지 판단하는 변수 Setter
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void SetCombatEnabled(bool bEnabled);

    UFUNCTION(BlueprintPure, Category = "Combat")
    bool CanMove() const;

    UFUNCTION(BlueprintPure, Category = "Combat")
    bool CanDodge() const;

public:
    // 전투 상태 변화 델리게이트
    UPROPERTY(BlueprintAssignable, Category = "Combat")
    FCombatActionStateChangedSignature OnCombatStateChanged;

protected:
	virtual void BeginPlay() override;

protected:
    // 무기 관리 컴포넌트 참조
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UWeaponComponent> WeaponComponent;

    // 스탯 컴포넌트 참조
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UStatComponent> StatComponent;

    // 근접 전투 컴포넌트 참조
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UMeleeCombatComponent> MeleeCombatComponent;

    // 원거리 전투 컴포넌트 참조
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<URangedCombatComponent> RangedCombatComponent;

    // 캐릭터의 현재 전투 액션 상태
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    ECombatActionState CurrentActionState;

    // 대쉬(회피) 세기
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float DashStrength;

    // 대쉬 스태미너 소모량
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float DashStaminaCost;

    // 락온한 적이 있는지 판단
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bIsLockedOn;

    // 조준 상태인지 판단
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bIsAiming;

    // 전투 가능한 상태인지 판단
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bCombatEnabled;

private:
    // 실행할 함수를 람다로 받아서 처리하는 근접 공격 공통 처리 함수
    void TryMeleeAttack(
        TFunctionRef<float(const AMeleeWeaponBase*)> GetStaminaCost,
        TFunctionRef<bool(AMeleeWeaponBase*)> CanComboAttackFunc,
        TFunctionRef<bool(AMeleeWeaponBase*, float)> AttackFunc
        );

    void SetActionState(ECombatActionState NewState);

    // 이전 동작을 끊고 회피 수행
    void InterruptActionForDodge();

    // 사망 시 행동 처리할 함수
    UFUNCTION()
    void HandleDead(AController* DeadInstigator);

    // 근접 공격 종료 시 처리할 함수
    UFUNCTION()
    void HandleMeleeAttackFinished();

    // 재장전 종료 시 처리할 함수
    UFUNCTION()
    void HandleReloadFinished();
};
