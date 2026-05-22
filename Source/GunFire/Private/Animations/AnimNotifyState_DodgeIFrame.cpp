#include "Animations/AnimNotifyState_DodgeIFrame.h"

#include "PlayerCharacter.h"
#include "StatComponent.h"

void UAnimNotifyState_DodgeIFrame::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                               float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    if (!IsValid(MeshComp)) return;

    AActor* Owner = MeshComp->GetOwner();
    if (!IsValid(Owner)) return;

    UStatComponent* StatComponent = Owner->FindComponentByClass<UStatComponent>();
    if (!IsValid(StatComponent)) return;

    StatComponent->SetInvincible(true);
}

void UAnimNotifyState_DodgeIFrame::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
    const FAnimNotifyEventReference& EventReference)
{
    if (!IsValid(MeshComp)) return;

    AActor* Owner = MeshComp->GetOwner();
    if (!IsValid(Owner)) return;

    UStatComponent* StatComponent = Owner->FindComponentByClass<UStatComponent>();
    if (!IsValid(StatComponent)) return;

    StatComponent->SetInvincible(false);
}
