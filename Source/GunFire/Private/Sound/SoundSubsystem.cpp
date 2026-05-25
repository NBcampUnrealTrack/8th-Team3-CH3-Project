#include "Sound/SoundSubsystem.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

void USoundSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    CurrentBGM = nullptr;
}

void USoundSubsystem::Deinitialize()
{
    StopBGM(0.f, true);

    Super::Deinitialize();
}

void USoundSubsystem::PlayBGM(USoundBase* BGM, float FadeInTime)
{
    if (!BGM) return;

    // 기존 bgm 이 재생중이라면 종료
    if (IsValid(CurrentBGM))
    {
        CurrentBGM->FadeOut(0.5f, 0.f);
    }

    CurrentBGM = UGameplayStatics::SpawnSound2D(
        GetWorld(),
        BGM,
        1.f,
        1.f,
        0.f,
        nullptr,
        true,
        false
        );

    if (IsValid(CurrentBGM))
    {
        CurrentBGM->FadeIn(FadeInTime);
    }

}

void USoundSubsystem::StopBGM(float FadeOutTime, bool bImmediate)
{
    if (!IsValid(CurrentBGM)) return;

    if (bImmediate || FadeOutTime <= 0.f)
    {
        CurrentBGM->Stop();
    }
    else
    {
        CurrentBGM->FadeOut(FadeOutTime, 0.f);
    }

    CurrentBGM = nullptr;
}
