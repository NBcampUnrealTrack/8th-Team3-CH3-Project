#include "Combat/RangedCombatComponent.h"

#include "Weapon/GunBase.h"

URangedCombatComponent::URangedCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void URangedCombatComponent::BeginPlay()
{
    Super::BeginPlay();

}

void URangedCombatComponent::TryFire(AGunBase* Gun, float AttackPower)
{
    if (!IsValid(Gun)) return;

    Gun->Attack();
}

bool URangedCombatComponent::TryReload(AGunBase* Gun)
{
    if (!IsValid(Gun)) return false;

    Gun->Reload();

    return true;
}

void URangedCombatComponent::FinishReload()
{
    OnReloadFinished.Broadcast();
}

