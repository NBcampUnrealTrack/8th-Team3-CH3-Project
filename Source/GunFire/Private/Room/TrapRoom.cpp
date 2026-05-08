#include "Room/TrapRoom.h"

ATrapRoom::ATrapRoom()
{
    RoomType = ERoomType::Trap;
}

void ATrapRoom::OnStart(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState)
{
    UE_LOG(LogTemp, Warning, TEXT("Trap Room Start"));
}

void ATrapRoom::OnEnd(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState)
{
    UE_LOG(LogTemp, Warning, TEXT("Trap Room End"));
}
