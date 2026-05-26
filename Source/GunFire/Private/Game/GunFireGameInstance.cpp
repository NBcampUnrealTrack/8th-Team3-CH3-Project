#include "Game/GunFireGameInstance.h"

UGunFireGameInstance::UGunFireGameInstance()
{
    EnemyStatBonusPerRoom = 0.05f;
    ResetSessionData();
}

void UGunFireGameInstance::StartNewSession()
{
    ResetSessionData();
}

void UGunFireGameInstance::ResetSessionData()
{
    PlayerSessionData = FPlayerSessionData();
    EquippedWeapons.Empty();
    InventorySessionData = FInventorySessionData();

    CurrentFloor = 1;
    ClearedRoomCount = 0;
    TotalClearedCombatRoomCount = 0;
    KilledEnemyCount = 0;
}

void UGunFireGameInstance::AddClearedRoomCount(int32 Count)
{
    ClearedRoomCount += FMath::Max(0, Count);
}

void UGunFireGameInstance::AddTotalClearedCombatRoomCount(int32 Count)
{
    TotalClearedCombatRoomCount += FMath::Max(0, Count);
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

const TArray<FEquippedWeaponSessionData>& UGunFireGameInstance::GetEquippedWeaponSessionData() const
{
    return EquippedWeapons;
}

const FInventorySessionData& UGunFireGameInstance::GetInventorySessionData() const
{
    return InventorySessionData;
}

int32 UGunFireGameInstance::GetCurrentFloor() const
{
    return CurrentFloor;
}

float UGunFireGameInstance::GetEnemyStatBonusRate() const
{
    return TotalClearedCombatRoomCount * EnemyStatBonusPerRoom;
}

void UGunFireGameInstance::SetPlayerSessionData(const FPlayerSessionData& SessionData)
{
    PlayerSessionData = SessionData;
}

void UGunFireGameInstance::SetEquippedWeaponSessionData(const TArray<FEquippedWeaponSessionData>& SessionData)
{
    EquippedWeapons = SessionData;
}

void UGunFireGameInstance::SetInventorySessionData(const FInventorySessionData& SessionData)
{
    InventorySessionData = SessionData;
}

void UGunFireGameInstance::SetCurrentFloor(int32 Floor)
{
    CurrentFloor = FMath::Max(1, Floor);
}
