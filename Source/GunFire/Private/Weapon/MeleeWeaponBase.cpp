#include "Weapon/MeleeWeaponBase.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Combat/MeleeCombatComponent.h"

AMeleeWeaponBase::AMeleeWeaponBase()
{
    // WeaponBase의 변수
    StaminaCost = 10.f;

    // MeleeWeapon 변수
    LightComboAnimMontage = nullptr;
    HeavyComboAnimMontage = nullptr;

    LightComboSectionNames = { TEXT("Basic1") };
    HeavyComboSectionNames = { TEXT("Smash1") };

    LightComboDamageRates = { 1.f };
    HeavyComboDamageRates = { 1.2f };

    HeavyAttackStaminaCost = 20.f;

    TraceStartSocketName = TEXT("TraceStart");
    TraceEndSocketName = TEXT("TraceEnd");

    TrailEffect = nullptr;
    TrailEffectSocketName = TEXT("TraceEnd");

    TraceRadius = 20.f;
}

void AMeleeWeaponBase::StartTrailEffect()
{
    if (!TrailEffect) return;

    UStaticMeshComponent* MeshComp = GetMesh();
    if (!IsValid(MeshComp)) return;

    // 켜져있을 수 있으니 꺼주기
    StopTrailEffect();

    TrailComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
        TrailEffect,
        MeshComp,
        TrailEffectSocketName,
        FVector::ZeroVector,
        FRotator::ZeroRotator,
        EAttachLocation::SnapToTarget,
        true
        );
}

void AMeleeWeaponBase::StopTrailEffect()
{
    if (!IsValid(TrailComponent)) return;

    TrailComponent->Deactivate();
    TrailComponent = nullptr;
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

float AMeleeWeaponBase::GetDamageRate() const
{
    return GetDamageRate(EMeleeAttackType::Light, 0);
}

float AMeleeWeaponBase::GetDamageRate(EMeleeAttackType AttackType, int32 ComboIndex) const
{
    const TArray<float> DamageRates =
        AttackType == EMeleeAttackType::Light
            ? LightComboDamageRates
            : HeavyComboDamageRates;

    if (!DamageRates.IsValidIndex(ComboIndex))
    {
        return Super::GetDamageRate();
    }

    return DamageRates[ComboIndex];
}
