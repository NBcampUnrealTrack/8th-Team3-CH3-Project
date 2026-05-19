#include "Animations/AnimNotifyState_MoveForward.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UAnimNotifyState_MoveForward::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                              float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

    if (!IsValid(MeshComp)) return;

    ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner());
    if (!IsValid(Character)) return;

    UCharacterMovementComponent* CharacterMovement = Character->GetCharacterMovement();
    if (!IsValid(CharacterMovement)) return;

    FVector MoveDelta = Character->GetActorForwardVector() * MoveSpeed * FrameDeltaTime;

    FHitResult HitResult;
    CharacterMovement->SafeMoveUpdatedComponent(
        MoveDelta,
        Character->GetActorRotation(),
        true,
        HitResult
        );
}
