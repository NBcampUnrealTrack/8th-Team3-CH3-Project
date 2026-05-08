#include "Game/GunFireGameInstance.h"

UGunFireGameInstance::UGunFireGameInstance()
{
    ResetSessionData();
}

void UGunFireGameInstance::StartNewSession()
{
    ResetSessionData();
}

void UGunFireGameInstance::FinishSession(ESessionResult Result)
{
    SessionResult = Result;
    // 세션 종료 시 필요한거
}

void UGunFireGameInstance::ResetSessionData()
{
    PlayerSessionData = FPlayerSessionData();
    CurrentFloor = 1;
    SessionResult = ESessionResult::None;
    RoomData.Empty();
    ClearedRoomCount = 0;
    KilledEnemyCount = 0;
}

void UGunFireGameInstance::AddRoomData(const FRoomData& Data)
{
    RoomData.Add(Data);
    ++ClearedRoomCount;
}

void UGunFireGameInstance::AddKilledEnemyCount(int32 Count)
{
    KilledEnemyCount += FMath::Max(0, Count);
}

void UGunFireGameInstance::MoveNextFloor()
{
    ++CurrentFloor;
}

const FPlayerSessionData& UGunFireGameInstance::GetPlayerSessionData() const
{
    return PlayerSessionData;
}

int32 UGunFireGameInstance::GetCurrentFloor() const
{
    return CurrentFloor;
}

ESessionResult UGunFireGameInstance::GetSessionResult() const
{
    return SessionResult;
}

void UGunFireGameInstance::SetPlayerSessionData(const FPlayerSessionData& SessionData)
{
    PlayerSessionData = SessionData;
}

void UGunFireGameInstance::SetCurrentFloor(int32 Floor)
{
    CurrentFloor = FMath::Max(1, Floor);
}
