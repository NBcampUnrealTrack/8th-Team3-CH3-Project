#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SessionData.h"
#include "GunFireGameInstance.generated.h"

// 저장해야 하는 정보
// 1. 진행정보  2. 플레이어 스탯   3. 무기/총알 상태  4. 인벤토리

UCLASS()
class GUNFIRE_API UGunFireGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
    UGunFireGameInstance();

    // 세션 시작
    UFUNCTION(BlueprintCallable, Category = "Session")
    void StartNewSession();

    // 세션정보 초기화
    UFUNCTION(BlueprintCallable, Category = "Session")
    void ResetSessionData();

    // 클리어한 방 개수 추가
    UFUNCTION(BlueprintCallable, Category = "Session")
    void AddClearedRoomCount(int32 Count = 1);

    // 클리어한 전투방 개수 추가
    UFUNCTION(BlueprintCallable, Category = "Session")
    void AddTotalClearedCombatRoomCount(int32 Count = 1);

    // 처치한 적 수 기록
    UFUNCTION(BlueprintCallable, Category = "Session")
    void AddKilledEnemyCount(int32 Count);

    // 현재 층 ++
    UFUNCTION(BlueprintCallable, Category = "Session")
    void MoveNextFloor();


    /* 게터 */

    UFUNCTION(BlueprintPure, Category = "Session")
    const FPlayerSessionData& GetPlayerSessionData() const;

    UFUNCTION(BlueprintPure, Category = "Session")
    const TArray<FEquippedWeaponSessionData>& GetEquippedWeaponSessionData() const;

    UFUNCTION(BlueprintPure, Category = "Session")
    const FInventorySessionData& GetInventorySessionData() const;


    UFUNCTION(BlueprintPure, Category = "Session")
    int32 GetCurrentFloor() const;

    UFUNCTION(BlueprintPure, Category = "Session")
    float GetEnemyStatBonusRate() const;

    /* 세터 */

    UFUNCTION(BlueprintCallable, Category = "Session")
    void SetPlayerSessionData(const FPlayerSessionData& SessionData);

    UFUNCTION(BlueprintCallable, Category = "Session")
    void SetEquippedWeaponSessionData(const TArray<FEquippedWeaponSessionData>& SessionData);

    UFUNCTION(BlueprintCallable, Category = "Session")
    void SetInventorySessionData(const FInventorySessionData& SessionData);

    UFUNCTION(BlueprintCallable, Category = "Session")
    void SetCurrentFloor(int32 Floor);


protected:
    // 저장할 플레이어 데이터
    UPROPERTY(BlueprintReadOnly, Category = "Session")
    FPlayerSessionData PlayerSessionData;

    // 저장할 무기 데이터
    UPROPERTY(BlueprintReadOnly, Category = "Session")
    TArray<FEquippedWeaponSessionData> EquippedWeapons;

    // 저장할 인벤토리 데이터
    UPROPERTY(BlueprintReadOnly, Category = "Session")
    FInventorySessionData InventorySessionData;

    // 현재 층
    UPROPERTY(BlueprintReadOnly, Category = "Session")
    int32 CurrentFloor;

    // 클리어한 방 갯수
    UPROPERTY(BlueprintReadOnly, Category = "Session")
    int32 ClearedRoomCount;

    // 클리어한 총 전투방 갯수
    UPROPERTY(BlueprintReadOnly, Category = "Session")
    int32 TotalClearedCombatRoomCount;

    // 처치한 적의 수
    UPROPERTY(BlueprintReadOnly, Category = "Session")
    int32 KilledEnemyCount;

    // 전투방을 클리어할때 증가할 적 스탯 보너스 수치
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Session")
    float EnemyStatBonusPerRoom;
};
