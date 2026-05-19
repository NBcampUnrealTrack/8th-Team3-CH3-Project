#include "Animations/AnimNotifyState_DirectionInput.h"

#include "Combat/MeleeCombatComponent.h"

void UAnimNotifyState_DirectionInput::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                 float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

    if (!IsValid(MeshComp)) return;

    AActor* Owner = MeshComp->GetOwner();
    if (!IsValid(Owner)) return;

    UMeleeCombatComponent* MeleeCombatComponent = Owner->FindComponentByClass<UMeleeCombatComponent>();
    if (!IsValid(MeleeCombatComponent)) return;

    MeleeCombatComponent->UpdateAttackDirection();
}
