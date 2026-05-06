#include "GunFireGameState.h"

AGunFireGameState::AGunFireGameState()
{
    CurrentRoomType = ERoomType::Safe;
    CurrentRoomState = ERoomState::Waiting;
    CurrentRoomIndex = 0;
    CurrentRoomID = NAME_None;
    RemainingEnemyCount = 0;
    CurrentFloor = 1;
    bPortalActivated = false;
}

ERoomType AGunFireGameState::GetCurrentRoomType() const
{
    return CurrentRoomType;
}

ERoomState AGunFireGameState::GetCurrentRoomState() const
{
    return CurrentRoomState;
}

int32 AGunFireGameState::GetCurrentRoomIndex() const
{
    return CurrentRoomIndex;
}

FName AGunFireGameState::GetCurrentRoomID() const
{
    return CurrentRoomID;
}

int32 AGunFireGameState::GetRemainingEnemyCount() const
{
    return RemainingEnemyCount;
}

int32 AGunFireGameState::GetCurrentFloor() const
{
    return CurrentFloor;
}

bool AGunFireGameState::GetPortalActivated() const
{
    return bPortalActivated;
}

void AGunFireGameState::SetCurrentRoomType(ERoomType Type)
{
    CurrentRoomType = Type;
}

void AGunFireGameState::SetCurrentRoomState(ERoomState State)
{
    CurrentRoomState = State;
}

void AGunFireGameState::SetCurrentRoomIndex(int32 Index)
{
    CurrentRoomIndex = FMath::Max(0, Index);
}

void AGunFireGameState::SetCurrentRoomID(FName ID)
{
    CurrentRoomID = ID;
}

void AGunFireGameState::SetRemainingEnemyCount(int32 Count)
{
    RemainingEnemyCount = FMath::Max(0, Count);
}

void AGunFireGameState::SetCurrentFloor(int32 Floor)
{
    CurrentFloor = FMath::Max(1, Floor);
}

void AGunFireGameState::SetPortalActivated(bool Activated)
{
    bPortalActivated = Activated;
}
