#include "Room/StartRoom.h"

#include "GunFire/GunFireGameMode.h"
#include "Interactables/Portal.h"

AStartRoom::AStartRoom()
{
    RoomType = ERoomType::Start;
    bEndImmediately = false;
}

void AStartRoom::BeginPlay()
{
    Super::BeginPlay();

    if (IsValid(FloorPortal))
    {
        FloorPortal->SetActive(false);
    }
}

void AStartRoom::ActivateFloorPortal()
{
    if (IsValid(FloorPortal))
    {
        FloorPortal->SetActive(true);
    }
}

AActor* AStartRoom::GetPlayerSpawnPoint() const
{
    return PlayerSpawnPoint.Get();
}

void AStartRoom::OnStart(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState)
{
    UE_LOG(LogTemp, Warning, TEXT("Start Room OnStart!!"));

    // 즉시 종료해야 한다면 시작하면서 EndRoom 호출
    if (bEndImmediately)
    {
        GFGameMode->EndStartRoom();
    }
}

void AStartRoom::OnEnd(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState)
{
    UE_LOG(LogTemp, Warning, TEXT("Start Room OnEnd!!"));
}
