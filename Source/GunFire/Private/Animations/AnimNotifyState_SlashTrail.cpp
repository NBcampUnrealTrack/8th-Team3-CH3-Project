#include "Animations/AnimNotifyState_SlashTrail.h"

#include "Weapon/MeleeWeaponBase.h"
#include "Weapon/WeaponComponent.h"

void UAnimNotifyState_SlashTrail::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                              float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    if (!IsValid(MeshComp)) return;

    AActor* Owner = MeshComp->GetOwner();
    if (!IsValid(Owner)) return;

    UWeaponComponent* WeaponComponent = Owner->GetComponentByClass<UWeaponComponent>();
    if (!IsValid(WeaponComponent)) return;

    AMeleeWeaponBase* MeleeWeapon = WeaponComponent->GetCurrentMeleeWeapon();
    if (!IsValid(MeleeWeapon)) return;

    MeleeWeapon->StartTrailEffect();
}

void UAnimNotifyState_SlashTrail::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
    const FAnimNotifyEventReference& EventReference)
{
    if (!IsValid(MeshComp)) return;

    AActor* Owner = MeshComp->GetOwner();
    if (!IsValid(Owner)) return;

    UWeaponComponent* WeaponComponent = Owner->GetComponentByClass<UWeaponComponent>();
    if (!IsValid(WeaponComponent)) return;

    AMeleeWeaponBase* MeleeWeapon = WeaponComponent->GetCurrentMeleeWeapon();
    if (!IsValid(MeleeWeapon)) return;

    MeleeWeapon->StopTrailEffect();
}
