#pragma once

#include "CoreMinimal.h"
#include "Weapon/WeaponBase.h"
#include "MeleeWeaponBase.generated.h"


UCLASS()
class GUNFIRE_API AMeleeWeaponBase : public AWeaponBase
{
	GENERATED_BODY()

public:
    AMeleeWeaponBase();

    UFUNCTION(BlueprintPure)
    UAnimMontage* GetHeavyComboAnimation(int32 ComboIndex) const;

    UFUNCTION(BlueprintPure)
    UAnimMontage* GetLightComboAnimation(int32 ComboIndex) const;

    UFUNCTION(BlueprintPure)
    FName GetTraceStartSocketName() const;

    UFUNCTION(BlueprintPure)
    FName GetTraceEndSocketName() const;

protected:
    // 강공격 콤보 애니메이션
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Melee")
    TArray<TObjectPtr<UAnimMontage>> HeavyComboAnimations;

    // 약공격 콤보 애니메이션, 약공격 1타는 기본 공격 애니메이션
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Melee")
    TArray<TObjectPtr<UAnimMontage>> LightComboAnimations;

    // 무기 손잡이
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Melee")
    FName TraceStartSocketName;

    // 무기 끝
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Melee")
    FName TraceEndSocketName;
};
