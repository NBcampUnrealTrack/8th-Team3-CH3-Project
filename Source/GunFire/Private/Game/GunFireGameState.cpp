#include "Game/GunFireGameState.h"

AGunFireGameState::AGunFireGameState()
{
    Initialize();
}

void AGunFireGameState::Initialize()
{
    CurrentRoomType = ERoomType::Start;
    CurrentRoomState = ERoomState::Waiting;
    CurrentRoomID = NAME_None;
    RemainingEnemyCount = 0;
    CurrentFloor = 1;
    bPortalActivated = false;
    RequiredCombatRoomCount = 0;
    ClearedCombatRoomCount = 0;
}

void AGunFireGameState::StartFloor(int32 NewFloor, int32 CombatRoomCount)
{
    Initialize();
    CurrentRoomState = ERoomState::InProgress;
    CurrentFloor = NewFloor;
    RequiredCombatRoomCount = CombatRoomCount;

    OnCombatRoomChanged.Broadcast(ClearedCombatRoomCount, RequiredCombatRoomCount);
}

ERoomType AGunFireGameState::GetCurrentRoomType() const
{
    return CurrentRoomType;
}

ERoomState AGunFireGameState::GetCurrentRoomState() const
{
    return CurrentRoomState;
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

int32 AGunFireGameState::GetRequiredCombatRoomCount() const
{
    return RequiredCombatRoomCount;
}

int32 AGunFireGameState::GetClearedCombatRoomCount() const
{
    return ClearedCombatRoomCount;
}

void AGunFireGameState::SetCurrentRoomType(ERoomType Type)
{
    CurrentRoomType = Type;
}

void AGunFireGameState::SetCurrentRoomState(ERoomState State)
{
    CurrentRoomState = State;
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

void AGunFireGameState::SetRequiredCombatRoomCount(int32 Count)
{
    RequiredCombatRoomCount = FMath::Max(0, Count);
}

void AGunFireGameState::SetClearedCombatRoomCount(int32 Count)
{
    ClearedCombatRoomCount = FMath::Max(0, Count);

    OnCombatRoomChanged.Broadcast(ClearedCombatRoomCount, RequiredCombatRoomCount);
}

void AGunFireGameState::AddClearedCombatRoomCount()
{
    ++ClearedCombatRoomCount;
}
