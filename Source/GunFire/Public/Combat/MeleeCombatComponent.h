#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MeleeCombatComponent.generated.h"

class AEnemyBase;
class ACharacter;
class AMeleeWeaponBase;
class UAnimMontage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAttackFinishedSignature);

UENUM(BlueprintType)
enum class EMeleeAttackType : uint8
{
    Light       UMETA(DisplayName = "약공격"),
    Heavy       UMETA(DisplayName = "강공격")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUNFIRE_API UMeleeCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMeleeCombatComponent();

    // 공격할 수 있는지 확인하는 함수
    // 애니메이션, 콤보 인덱스, 무기 등의 유효성을 판단함
    UFUNCTION(BlueprintPure, Category = "Combat|Melee")
    bool CanStartAttack(AMeleeWeaponBase* MeleeWeapon, EMeleeAttackType AttackType, int32 ComboIndex) const;

    // 공격 입력을 받을 수 있는지 확인하는 함수
    UFUNCTION(BlueprintPure, Category = "Combat|Melee")
    bool CanAcceptAttackInput(AMeleeWeaponBase* MeleeWeapon, EMeleeAttackType AttackType) const;

    // 현재 공격중인지 확인하는 함수
    UFUNCTION(BlueprintPure, Category = "Combat|Melee")
    bool IsAttackInProgress() const;

    // 현재 무기가 유효한지 확인하는 함수
    UFUNCTION(BlueprintPure, Category = "Combat|Melee")
    bool IsValidCurrentWeapon() const;

    // 무기, 공격타입, 콤보 인덱스를 받아서 애니메이션 몽타주를 가져오는 함수
    UFUNCTION(BlueprintPure, Category = "Combat|Melee")
    UAnimMontage* GetAttackAnimMontage(AMeleeWeaponBase* MeleeWeapon, EMeleeAttackType AttackType) const;

    // 약공격 시도
    UFUNCTION(BlueprintCallable, Category = "Combat|Melee")
    bool TryLightAttack(AMeleeWeaponBase* MeleeWeapon, float AttackPower);

    // 강공격 시도
    UFUNCTION(BlueprintCallable, Category = "Combat|Melee")
    bool TryHeavyAttack(AMeleeWeaponBase* MeleeWeapon, float AttackPower);

    // 공격 종료시 처리하는 내용
    UFUNCTION(BlueprintCallable, Category = "Combat|Melee")
    void FinishAttack();

    // 근접무기 Trace 시작
    UFUNCTION(BlueprintCallable, Category = "Combat|Melee")
    void BeginAttackTrace();

    // 근접무기 Trace 끝
    UFUNCTION(BlueprintCallable, Category = "Combat|Melee")
    void EndAttackTrace();

    // 콤보 입력을 받도록 처리하는 함수
    UFUNCTION(BlueprintCallable, Category = "Combat|Melee")
    void OpenComboInput();

    // 콤보 입력을 받지 못하게 처리하는 함수
    UFUNCTION(BlueprintCallable, Category = "Combat|Melee")
    void CloseComboInput();

public:
    UPROPERTY(BlueprintAssignable)
    FAttackFinishedSignature OnAttackFinished;

protected:
	virtual void BeginPlay() override;

protected:
    // 컴포넌트 소유자
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Melee")
    TObjectPtr<ACharacter> OwnerCharacter;

    // 현재 공격 타입
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Melee")
    EMeleeAttackType CurrentAttackType;

    // 현재 콤보 인덱스
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Melee")
    int32 CurrentComboIndex;

    // 콤보를 이어갈 입력을 받을 수 있는지 판단하는 변수
    // Anim Notify로 타이밍을 제어해 이 변수값을 변경할 것
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Melee")
    bool bCanComboInput;

    // 콤보 입력이 들어왔는지 판단하는 변수
    // 판정 구간내에 입력이 들어오면 다음 섹션으로 넘어가게 함
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Melee")
    bool bComboTriggered;

    // 현재 공격중인지를 판단하는 변수
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Melee")
    bool bAttackInProgress;

private:
    // 공격 시도 함수
    bool TryAttack(AMeleeWeaponBase* MeleeWeapon, float AttackPower, EMeleeAttackType AttackType);
    // 첫 공격 시작 처리 함수
    bool StartAttack(AMeleeWeaponBase* MeleeWeapon, float AttackPower, EMeleeAttackType AttackType);
    // 콤보 공격 시도 함수
    bool TryChainAttack(AMeleeWeaponBase* MeleeWeapon, float AttackPower, EMeleeAttackType AttackType);

    // 현재 콤보 인덱스에 해당하는 콤보 공격 섹션 가져오는 함수
    FName GetComboSectionName(AMeleeWeaponBase* MeleeWeapon, EMeleeAttackType AttackType, int32 ComboIndex) const;

    // 공격 상태 초기화하는 함수
    void ResetAttackState();

    // 공격 사운드 재생 함수
    void PlayAttackSound(AMeleeWeaponBase* MeleeWeapon) const;
    // 공격 애니메이션 몽타주 끝났을때 처리하는 함수
    UFUNCTION()
    void HandleAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    // 공격 시 충돌 트레이스 처리하는 함수
    void DoHitTrace();

private:
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
