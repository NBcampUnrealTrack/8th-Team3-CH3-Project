#include "Weapon/MeleeWeaponBase.h"

AMeleeWeaponBase::AMeleeWeaponBase()
{
    DamageRate = 1.f;
    StaminaCost = 10.f;
    HeavyAttackStaminaCost = 20.f;
    TraceRadius = 20.f;

    TraceStartSocketName = TEXT("TraceStart");
    TraceEndSocketName = TEXT("TraceEnd");
}

UAnimMontage* AMeleeWeaponBase::GetHeavyComboAnimation(int32 ComboIndex) const
{
    if (HeavyComboAnimations.IsValidIndex(ComboIndex))
    {
        return HeavyComboAnimations[ComboIndex].Get();
    }

    return nullptr;
}

UAnimMontage* AMeleeWeaponBase::GetLightComboAnimation(int32 ComboIndex) const
{
    if (LightComboAnimations.IsValidIndex(ComboIndex))
    {
        return LightComboAnimations[ComboIndex].Get();
    }

    return nullptr;
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
