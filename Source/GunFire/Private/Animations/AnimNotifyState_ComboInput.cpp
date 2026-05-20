#include "Animations/AnimNotifyState_ComboInput.h"

#include "Combat/MeleeCombatComponent.h"

void UAnimNotifyState_ComboInput::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
    float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    if (!IsValid(MeshComp)) return;

    AActor* Owner = MeshComp->GetOwner();
    if (!IsValid(Owner)) return;

    UMeleeCombatComponent* MeleeCombatComponent = Owner->FindComponentByClass<UMeleeCombatComponent>();
    if (!IsValid(MeleeCombatComponent)) return;

    MeleeCombatComponent->OpenComboInput();
}

void UAnimNotifyState_ComboInput::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
    const FAnimNotifyEventReference& EventReference)
{
    if (!IsValid(MeshComp)) return;

    AActor* Owner = MeshComp->GetOwner();
    if (!IsValid(Owner)) return;

    UMeleeCombatComponent* MeleeCombatComponent = Owner->FindComponentByClass<UMeleeCombatComponent>();
    if (!IsValid(MeleeCombatComponent)) return;

    MeleeCombatComponent->CloseComboInput();
}
