#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_AttackTurn.generated.h"


UCLASS()
class GUNFIRE_API UAnimNotifyState_AttackTurn : public UAnimNotifyState
{
	GENERATED_BODY()

public:
    virtual void NotifyTick(
        USkeletalMeshComponent* MeshComp,
        UAnimSequenceBase* Animation,
        float FrameDeltaTime,
        const FAnimNotifyEventReference& EventReference) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float TurnSpeed = 20.f;
};
