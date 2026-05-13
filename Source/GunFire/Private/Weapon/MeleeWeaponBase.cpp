#include "Weapon/MeleeWeaponBase.h"

AMeleeWeaponBase::AMeleeWeaponBase()
{
    AttachSocketName = TEXT("RightGrip");
    DamageRate = 1.f;
    TraceStartSocketName = TEXT("TraceStart");
    TraceEndSocketName = TEXT("TraceEnd");
}

UAnimMontage* AMeleeWeaponBase::GetHeavyComboAnimation(int32 ComboIndex) const
{
    if (HeavyComboAnimations.IsValidIndex(ComboIndex))
    {
        return HeavyComboAnimations[ComboIndex].Get();
    }

    return AttackAnimation.Get();
}

UAnimMontage* AMeleeWeaponBase::GetLightComboAnimation(int32 ComboIndex) const
{
    if (LightComboAnimations.IsValidIndex(ComboIndex))
    {
        return LightComboAnimations[ComboIndex].Get();
    }

    return AttackAnimation.Get();
}

FName AMeleeWeaponBase::GetTraceStartSocketName() const
{
    return TraceStartSocketName;
}

FName AMeleeWeaponBase::GetTraceEndSocketName() const
{
    return TraceEndSocketName;
}
