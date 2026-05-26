#include "Room/BossRoom.h"

#include "Interactables/Portal.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

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
        BossThemeComponent = UGameplayStatics::SpawnSound2D(this, BossTheme);
    }
    else
    {
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
