#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Room/RoomTypes.h"
#include "GunFireGameState.generated.h"


UCLASS()
class GUNFIRE_API AGunFireGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
    AGunFireGameState();

    // 초기화 함수
    UFUNCTION(BlueprintCallable)
    void Initialize();

    // 새로운 층, 남은 전투방 개수를 받아서 층을 시작하는 함수
    UFUNCTION(BlueprintCallable)
    void StartFloor(int32 NewFloor, int32 CombatRoomCount);

    UFUNCTION(BlueprintPure)
    ERoomType GetCurrentRoomType() const;
    UFUNCTION(BlueprintPure)
    ERoomState GetCurrentRoomState() const;
    UFUNCTION(BlueprintPure)
    FName GetCurrentRoomID() const;
    UFUNCTION(BlueprintPure)
    int32 GetRemainingEnemyCount() const;
    UFUNCTION(BlueprintPure)
    int32 GetCurrentFloor() const;
    UFUNCTION(BlueprintPure)
    bool GetPortalActivated() const;
    UFUNCTION(BlueprintPure)
    int32 GetRequiredCombatRoomCount() const;
    UFUNCTION(BlueprintPure)
    int32 GetClearedCombatRoomCount() const;

    void SetCurrentRoomType(ERoomType Type);
    void SetCurrentRoomState(ERoomState State);
    void SetCurrentRoomID(FName ID);
    void SetRemainingEnemyCount(int32 Count);
    void SetCurrentFloor(int32 Floor);
    void SetPortalActivated(bool Activated);
    void SetRequiredCombatRoomCount(int32 Count);
    void SetClearedCombatRoomCount(int32 Count);

    void AddClearedCombatRoomCount();

protected:
    // 현재 방의 타입
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
    ERoomType CurrentRoomType;

    // 현재 방의 상태
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
    ERoomState CurrentRoomState;

    // 현재 방 ID
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
    FName CurrentRoomID;

    // 남은 적의 수
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
    int32 RemainingEnemyCount;

    // 현재 층
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
    int32 CurrentFloor;

    // 포탈 활성화 상태
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
    bool bPortalActivated;

    // 다음 층으로 이동하는데 필요한 전투방 클리어 횟수
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
    int32 RequiredCombatRoomCount;

    // 현재 클리어한 전투방 횟수
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
    int32 ClearedCombatRoomCount;
};
