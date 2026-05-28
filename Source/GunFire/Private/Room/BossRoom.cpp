#include "Room/BossRoom.h"

#include "Interactables/Portal.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundSubsystem.h"

ABossRoom::ABossRoom()
{
    RoomType = ERoomType::Boss;
    Initialize();
}

void ABossRoom::ActivateResultPortal()
{
    if (IsValid(ResultPortal))
    {
        ResultPortal->SetActive(true);
    }
}

void ABossRoom::OnPrepare(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState)
{
    ACombatRoom::OnPrepare(GFGameMode, GFGameState);

    if (IsValid(BossTheme))
    {
        // 기존 BGM은 종료
        USoundSubsystem* SoundSubsystem = GetWorld() ? GetWorld()->GetSubsystem<USoundSubsystem>() : nullptr;
        if (IsValid(SoundSubsystem))
        {
            SoundSubsystem->StopBGM(0.5f, false);
        }

        BossThemeComponent = UGameplayStatics::SpawnSound2D(this, BossTheme);
    }
}

void ABossRoom::OnEnd(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState)
{
    if (IsValid(BossThemeComponent))
    {
        BossThemeComponent->Stop();
        BossThemeComponent = nullptr;
    }

    ACombatRoom::OnEnd(GFGameMode, GFGameState);
}
