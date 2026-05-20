#include "Animations/AnimNotifyState_AttackTurn.h"

#include "PlayerCharacter.h"
#include "Combat/MeleeCombatComponent.h"

void UAnimNotifyState_AttackTurn::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                             float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
    if (!IsValid(MeshComp)) return;

    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(MeshComp->GetOwner());
    if (!IsValid(PlayerCharacter)) return;

    UMeleeCombatComponent* MeleeCombatComponent = PlayerCharacter->FindComponentByClass<UMeleeCombatComponent>();
    if (!IsValid(MeleeCombatComponent)) return;

    FRotator TargetRotation = MeleeCombatComponent->GetCurrentAttackRotation();
    FRotator CurrentRotation = PlayerCharacter->GetActorRotation();

    FRotator NewRotation = FMath::RInterpTo(
        CurrentRotation,
        TargetRotation,
        FrameDeltaTime,
        TurnSpeed
        );

    PlayerCharacter->SetActorRotation(FRotator(0.f, NewRotation.Yaw, 0.f));
}
