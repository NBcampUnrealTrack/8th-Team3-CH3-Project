#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_DirectionInput.generated.h"


UCLASS()
class GUNFIRE_API UAnimNotifyState_DirectionInput : public UAnimNotifyState
{
	GENERATED_BODY()

public:
    virtual void NotifyTick(
        USkeletalMeshComponent* MeshComp,
        UAnimSequenceBase* Animation,
        float FrameDeltaTime,
        const FAnimNotifyEventReference& EventReference) override;
};
