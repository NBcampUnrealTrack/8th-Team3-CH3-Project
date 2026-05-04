#include "Room/RoomBase.h"

ARoomBase::ARoomBase()
{
 	PrimaryActorTick.bCanEverTick = false;
}

void ARoomBase::StartRoom(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState)
{
    if (!GFGameMode || !GFGameState)
    {
        UE_LOG(LogTemp, Warning, TEXT("Game Mode or Game State Null"));
        return;
    }

    OnStart(GFGameMode, GFGameState);
}

void ARoomBase::OnStart(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState)
{
}

void ARoomBase::EndRoom(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState)
{
    if (!GFGameMode || !GFGameState)
    {
        UE_LOG(LogTemp, Warning, TEXT("Game Mode or Game State Null"));
        return;
    }

    OnEnd(GFGameMode, GFGameState);
}

void ARoomBase::OnEnd(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState)
{
}
