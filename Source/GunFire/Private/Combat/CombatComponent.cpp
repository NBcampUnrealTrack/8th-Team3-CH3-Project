#include "Combat/CombatComponent.h"

#include "StatComponent.h"
#include "Combat/MeleeCombatComponent.h"
#include "Combat/RangedCombatComponent.h"
#include "Weapon/GunBase.h"
#include "Weapon/MeleeWeaponBase.h"
#include "Weapon/WeaponComponent.h"

UCombatComponent::UCombatComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    WeaponComponent = nullptr;
    StatComponent = nullptr;
    MeleeCombatComponent = nullptr;
    RangedCombatComponent = nullptr;

    CurrentActionState = ECombatActionState::None;
    bIsLockedOn = false;
    bIsAiming = false;
    bCombatEnabled = true;
}

void UCombatComponent::BeginPlay()
{
    Super::BeginPlay();

    AActor* Owner = GetOwner();
    if (!IsValid(Owner)) return;

    // 실제로 사용할 컴포넌트 찾아서 참조함
    WeaponComponent = Owner->FindComponentByClass<UWeaponComponent>();
    StatComponent = Owner->FindComponentByClass<UStatComponent>();
    MeleeCombatComponent = Owner->FindComponentByClass<UMeleeCombatComponent>();
    RangedCombatComponent = Owner->FindComponentByClass<URangedCombatComponent>();

    if (IsValid(StatComponent))
    {
        StatComponent->OnDead.AddDynamic(this, &UCombatComponent::HandleDead);
    }
    if (IsValid(MeleeCombatComponent))
    {
        MeleeCombatComponent->OnAttackFinished.AddDynamic(this, &UCombatComponent::HandleMeleeAttackFinished);
    }
    if (IsValid(RangedCombatComponent))
    {
        RangedCombatComponent->OnReloadFinished.AddDynamic(this, &UCombatComponent::HandleReloadFinished);
    }
}

void UCombatComponent::TryLightAttack()
{
    TryMeleeAttack(
        [](const AMeleeWeaponBase* Weapon)
        {
            return Weapon->GetStaminaCost();
        },
        [this](AMeleeWeaponBase* Weapon, float AttackPower)
        {
            return MeleeCombatComponent->TryLightAttack(Weapon, AttackPower);
        }
        );
}

void UCombatComponent::TryHeavyAttack()
{
    TryMeleeAttack(
        [](const AMeleeWeaponBase* Weapon)
        {
            return Weapon->GetHeavyAttackStaminaCost();
        },
        [this](AMeleeWeaponBase* Weapon, float AttackPower)
        {
            return MeleeCombatComponent->TryHeavyAttack(Weapon, AttackPower);
        }
        );
}

void UCombatComponent::TryRangedAttack()
{
    if (!IsValid(RangedCombatComponent)) return;
    if (!IsValid(WeaponComponent)) return;
    if (!IsValid(StatComponent)) return;

    // 총이 유효한지 확인
    AGunBase* Gun = WeaponComponent->GetCurrentGun();
    if (!IsValid(Gun)) return;

    // 사격은 즉발이므로 따로 Attacking 상태로 전환 X
    if (!CanStartCombatAction()) return;

    float FinalAttackPower = StatComponent->GetAttackPower();

    RangedCombatComponent->TryFire(Gun, FinalAttackPower);
}

void UCombatComponent::TryReload()
{
    if (!IsValid(RangedCombatComponent) || !IsValid(WeaponComponent)) return;

    // 총이 유효한지 확인
    AGunBase* Gun = WeaponComponent->GetCurrentGun();
    if (!IsValid(Gun)) return;

    // 재장전 상태로 바꿀 수 있는지 확인하고 가능하면 재장전 상태로 전환
    if (!TrySetActionState(ECombatActionState::Reloading)) return;

    // 재장전 시도하고 실패 시 행동 상태 되돌리기
    if (!RangedCombatComponent->TryReload(Gun))
    {
        ClearActionState(ECombatActionState::Reloading);
    }
}

void UCombatComponent::TryDodge()
{
    if (!TrySetActionState(ECombatActionState::Dodging)) return;

    // 회피 행동 호출 or 처리
}

bool UCombatComponent::TrySetActionState(ECombatActionState NewState)
{
    if (!CanChangeActionState(NewState)) return false;

    SetActionState(NewState);
    return true;
}

void UCombatComponent::ClearActionState(ECombatActionState ExpectedState)
{
    if (CurrentActionState != ExpectedState) return;
    if (CurrentActionState == ECombatActionState::Dead) return;

    SetActionState(ECombatActionState::None);
}

void UCombatComponent::ForceSetActionState(ECombatActionState NewState)
{
    SetActionState(NewState);
}

void UCombatComponent::GetStunned()
{
    ForceSetActionState(ECombatActionState::Stunned);
}

void UCombatComponent::StartInteracting()
{
    TrySetActionState(ECombatActionState::Interacting);
}

bool UCombatComponent::CanStartCombatAction() const
{
    if (!bCombatEnabled) return false;

    // 현재 상태가 None일때만 다른 전투 행동으로 전환 가능
    // 하나의 행동이 종료되어야만 다른 행동으로 전환이 가능하게 됨
    switch (CurrentActionState)
    {
    case ECombatActionState::None:
        return true;

    default:
        return false;
    }
}

bool UCombatComponent::CanChangeActionState(ECombatActionState NewState) const
{
    if (CurrentActionState == ECombatActionState::Dead) return false;
    if (NewState == ECombatActionState::Dead) return true;
    if (!bCombatEnabled) return false;

    switch (NewState)
    {
    case ECombatActionState::None:
        return true;

    case ECombatActionState::Attacking:
    case ECombatActionState::Reloading:
    case ECombatActionState::Dodging:
    case ECombatActionState::UsingSkill:
    case ECombatActionState::Interacting:
        return CurrentActionState == ECombatActionState::None;

    case ECombatActionState::Stunned:
        return CurrentActionState != ECombatActionState::Dead;

    default:
        return false;
    }
}

ECombatActionState UCombatComponent::GetCurrentActionState() const
{
    return CurrentActionState;
}

bool UCombatComponent::IsCombatEnabled() const
{
    return bCombatEnabled;
}

void UCombatComponent::SetCombatEnabled(bool bEnabled)
{
    bCombatEnabled = bEnabled;
}

bool UCombatComponent::CanMove() const
{
    return bCombatEnabled
        && CurrentActionState != ECombatActionState::Attacking
        && CurrentActionState != ECombatActionState::Dodging
        && CurrentActionState != ECombatActionState::Stunned
        && CurrentActionState != ECombatActionState::Dead;
}

void UCombatComponent::TryMeleeAttack(
    TFunctionRef<float(const AMeleeWeaponBase*)> GetStaminaCost,
    TFunctionRef<bool(AMeleeWeaponBase*, float)> AttackFunc)
{
    if (!IsValid(MeleeCombatComponent)) return;
    if (!IsValid(WeaponComponent)) return;
    if (!IsValid(StatComponent)) return;

    // 근접무기가 유효한지 확인
    AMeleeWeaponBase* MeleeWeapon = WeaponComponent->GetCurrentMeleeWeapon();
    if (!IsValid(MeleeWeapon)) return;

    // 근접 무기 공격을 처리할 수 있는지 확인
    if (!MeleeCombatComponent->CanStartAttack(MeleeWeapon)) return;

    // 공격 상태로 바꿀 수 있는지 확인하고 가능하면 공격 상태로 전환
    if (!TrySetActionState(ECombatActionState::Attacking)) return;

    // 스태미너 사용 처리, 실패하면 행동도 되돌림
    const float Cost = GetStaminaCost(MeleeWeapon);
    if (!StatComponent->TryConsumeStamina(Cost))
    {
        ClearActionState(ECombatActionState::Attacking);
        return;
    }

    float FinalAttackPower = StatComponent->GetAttackPower();

    // 근접무기로 공격
    if (!AttackFunc(MeleeWeapon, FinalAttackPower))
    {
        ClearActionState(ECombatActionState::Attacking);
    }
}

void UCombatComponent::SetActionState(ECombatActionState NewState)
{
    if (CurrentActionState == NewState) return;

    ECombatActionState PrevState = CurrentActionState;
    CurrentActionState = NewState;

    OnCombatStateChanged.Broadcast(PrevState, CurrentActionState);
}

void UCombatComponent::HandleDead(AController* DeadInstigator)
{
    bCombatEnabled = false;
    ForceSetActionState(ECombatActionState::Dead);
}

void UCombatComponent::HandleMeleeAttackFinished()
{
    ClearActionState(ECombatActionState::Attacking);
}

void UCombatComponent::HandleReloadFinished()
{
    ClearActionState(ECombatActionState::Reloading);
}
