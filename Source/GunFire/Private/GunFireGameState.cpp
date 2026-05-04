#include "GunFireGameState.h"

AGunFireGameState::AGunFireGameState()
{
    CurrentRoomType = ERoomType::Combat;
    CurrentRoomState = ERoomState::Waiting;
    RemainingEnemyCount = 0;
    bPortalActivated = false;
}

ERoomType AGunFireGameState::GetRoomType() const
{
    return CurrentRoomType;
}

ERoomState AGunFireGameState::GetRoomState() const
{
    return CurrentRoomState;
}

int32 AGunFireGameState::GetRemainingEnemyCount() const
{
    return RemainingEnemyCount;
}

bool AGunFireGameState::GetPortalActivated() const
{
    return bPortalActivated;
}

void AGunFireGameState::SetRoomType(ERoomType Type)
{
    CurrentRoomType = Type;
}

void AGunFireGameState::SetRoomState(ERoomState State)
{
    CurrentRoomState = State;
}

void AGunFireGameState::SetRemainingEnemyCount(int32 Count)
{
    if (Count <= 0)
    {
        Count = 0;
    }
    RemainingEnemyCount = Count;
}

void AGunFireGameState::SetPortalActivated(bool Activated)
{
    bPortalActivated = Activated;
}
