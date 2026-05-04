#include "Room/TreasureRoom.h"

void ATreasureRoom::OnStart(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState)
{
    UE_LOG(LogTemp, Warning, TEXT("Treasure Room Start"));
}

void ATreasureRoom::OnEnd(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState)
{
    UE_LOG(LogTemp, Warning, TEXT("Treasure Room End"));
}
