#include "Weapon/MeleeWeaponBase.h"

AMeleeWeaponBase::AMeleeWeaponBase()
{
    // WeaponBase의 변수
    DamageRate = 1.f;
    StaminaCost = 10.f;

    // MeleeWeapon 변수
    LightComboAnimMontage = nullptr;
    HeavyComboAnimMontage = nullptr;

    LightComboSectionNames = { TEXT("Light_1") };
    HeavyComboSectionNames = { TEXT("Heavy_1") };

    HeavyAttackStaminaCost = 20.f;
    TraceStartSocketName = TEXT("TraceStart");
    TraceEndSocketName = TEXT("TraceEnd");
    TraceRadius = 20.f;
}

UAnimMontage* AMeleeWeaponBase::GetHeavyComboAnimMontage() const
{
    return HeavyComboAnimMontage.Get();
}

UAnimMontage* AMeleeWeaponBase::GetLightComboAnimMontage() const
{
    return LightComboAnimMontage.Get();
}

FName AMeleeWeaponBase::GetHeavyComboSectionName(int32 ComboIndex) const
{
    return HeavyComboSectionNames.IsValidIndex(ComboIndex)
        ? HeavyComboSectionNames[ComboIndex]
        : NAME_None;
}

FName AMeleeWeaponBase::GetLightComboSectionName(int32 ComboIndex) const
{
    return LightComboSectionNames.IsValidIndex(ComboIndex)
    ? LightComboSectionNames[ComboIndex]
    : NAME_None;
}

int32 AMeleeWeaponBase::GetHeavyComboCount() const
{
    return HeavyComboSectionNames.Num();
}

int32 AMeleeWeaponBase::GetLightComboCount() const
{
    return LightComboSectionNames.Num();
}

float AMeleeWeaponBase::GetHeavyAttackStaminaCost() const
{
    return HeavyAttackStaminaCost;
}

FName AMeleeWeaponBase::GetTraceStartSocketName() const
{
    return TraceStartSocketName;
}

FName AMeleeWeaponBase::GetTraceEndSocketName() const
{
    return TraceEndSocketName;
}

float AMeleeWeaponBase::GetTraceRadius() const
{
    return TraceRadius;
}
