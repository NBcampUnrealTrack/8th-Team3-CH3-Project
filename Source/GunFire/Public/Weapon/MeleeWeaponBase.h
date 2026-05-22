#pragma once

#include "CoreMinimal.h"
#include "Weapon/WeaponBase.h"
#include "MeleeWeaponBase.generated.h"


enum class EMeleeAttackType : uint8;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class GUNFIRE_API AMeleeWeaponBase : public AWeaponBase
{
	GENERATED_BODY()

public:
    AMeleeWeaponBase();

    // 공격 잔상 이펙트 켜기
    UFUNCTION(BlueprintCallable)
    void StartTrailEffect();

    // 공격 잔상 이펙트 끄기
    UFUNCTION(BlueprintCallable)
    void StopTrailEffect();

    // 강공격 애니메이션 몽타주 가져오는 함수
    UFUNCTION(BlueprintPure)
    UAnimMontage* GetHeavyComboAnimMontage() const;

    // 약공격 애니메이션 몽타주 가져오는 함수
    UFUNCTION(BlueprintPure)
    UAnimMontage* GetLightComboAnimMontage() const;

    // 애니메이션 몽타주에서 콤보에 해당하는 섹션 이름을 가져오는 함수
    UFUNCTION(BlueprintPure)
    FName GetHeavyComboSectionName(int32 ComboIndex) const;

    // 애니메이션 몽타주에서 콤보에 해당하는 섹션 이름을 가져오는 함수
    UFUNCTION(BlueprintPure)
    FName GetLightComboSectionName(int32 ComboIndex) const;

    // 강공격 콤보가 몇개까지 있는지 가져오는 함수
    UFUNCTION(BlueprintPure)
    int32 GetHeavyComboCount() const;

    // 약공격 콤보가 몇개까지 있는지 가져오는 함수
    UFUNCTION(BlueprintPure)
    int32 GetLightComboCount() const;

    UFUNCTION(BlueprintPure)
    float GetHeavyAttackStaminaCost() const;

    UFUNCTION(BlueprintPure)
    FName GetTraceStartSocketName() const;

    UFUNCTION(BlueprintPure)
    FName GetTraceEndSocketName() const;

    UFUNCTION(BlueprintPure)
    float GetTraceRadius() const;


    float GetDamageRate() const override;

    float GetDamageRate(EMeleeAttackType AttackType, int32 ComboIndex) const;

protected:
    // 약공격 콤보 애니메이션
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Melee")
    TObjectPtr<UAnimMontage> LightComboAnimMontage;

    // 강공격 콤보 애니메이션
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Melee")
    TObjectPtr<UAnimMontage> HeavyComboAnimMontage;

    // 약공격 콤보들의 애니메이션 몽타주 섹션 이름
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Melee")
    TArray<FName> LightComboSectionNames;

    // 강공격 콤보들의 애니메이션 몽타주 섹션 이름
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Melee")
    TArray<FName> HeavyComboSectionNames;

    // 약공격 콤보들의 데미지 배율
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Melee")
    TArray<float> LightComboDamageRates;

    // 강공격 콤보들의 데미지 배율
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Melee")
    TArray<float> HeavyComboDamageRates;

    // 강공격의 스태미너 소모량
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Stamina")
    float HeavyAttackStaminaCost;

    // 충돌 판정 트레이스 시작점
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Melee")
    FName TraceStartSocketName;

    // 충돌 판정 트레이스 끝점
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Melee")
    FName TraceEndSocketName;

    // 공격 잔상 연출용 나이아가라 이펙트 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Component")
    TObjectPtr<UNiagaraComponent> TrailComponent;

    // 공격 잔상 이펙트
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|VFX")
    TObjectPtr<UNiagaraSystem> TrailEffect;

    // 공격 잔상 이펙트 소켓 이름
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|VFX")
    FName TrailEffectSocketName;

    // 무기 두께
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Melee")
    float TraceRadius;
};
