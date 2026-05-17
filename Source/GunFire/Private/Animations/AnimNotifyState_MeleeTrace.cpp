#include "Animations/AnimNotifyState_MeleeTrace.h"

#include "Combat/MeleeCombatComponent.h"

void UAnimNotifyState_MeleeTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
    float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    if (!IsValid(MeshComp)) return;

    AActor* Owner = MeshComp->GetOwner();
    if (!IsValid(Owner)) return;

    UMeleeCombatComponent* MeleeCombatComponent = Owner->FindComponentByClass<UMeleeCombatComponent>();
    if (!IsValid(MeleeCombatComponent)) return;

    MeleeCombatComponent->BeginAttackTrace();
}

void UAnimNotifyState_MeleeTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
    const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyEnd(MeshComp, Animation, EventReference);

    if (!IsValid(MeshComp)) return;

    AActor* Owner = MeshComp->GetOwner();
    if (!IsValid(Owner)) return;

    UMeleeCombatComponent* MeleeCombatComponent = Owner->FindComponentByClass<UMeleeCombatComponent>();
    if (!IsValid(MeleeCombatComponent)) return;

    MeleeCombatComponent->EndAttackTrace();
}
