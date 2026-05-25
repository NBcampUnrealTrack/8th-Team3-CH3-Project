#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "SoundSubsystem.generated.h"


UCLASS()
class GUNFIRE_API USoundSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable)
    void PlayBGM(USoundBase* BGM, float FadeInTime = 1.f);

    UFUNCTION(BlueprintCallable)
    void StopBGM(float FadeOutTime = 1.f, bool bImmediate = true);

private:
    UPROPERTY()
    TObjectPtr<UAudioComponent> CurrentBGM;
};
