#include "Room/CombatRoom.h"

void ACombatRoom::OnStart(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState)
{
    UE_LOG(LogTemp, Warning, TEXT("Combat Room Start"));
}

void ACombatRoom::OnEnd(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState)
{
    // 몬스터 정리 필요
    UE_LOG(LogTemp, Warning, TEXT("Combat Room End"));
}
