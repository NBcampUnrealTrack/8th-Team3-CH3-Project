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

    UFUNCTION(BlueprintPure)
    ERoomType GetCurrentRoomType() const;
    UFUNCTION(BlueprintPure)
    ERoomState GetCurrentRoomState() const;
    UFUNCTION(BlueprintCallable)
    int32 GetCurrentRoomIndex() const;
    UFUNCTION(BlueprintCallable)
    FName GetCurrentRoomID() const;
    UFUNCTION(BlueprintPure)
    int32 GetRemainingEnemyCount() const;
    UFUNCTION(BlueprintCallable)
    int32 GetCurrentFloor() const;
    UFUNCTION(BlueprintPure)
    bool GetPortalActivated() const;

    void SetCurrentRoomType(ERoomType Type);
    void SetCurrentRoomState(ERoomState State);
    void SetCurrentRoomIndex(int32 Index);
    void SetCurrentRoomID(FName ID);
    void SetRemainingEnemyCount(int32 Count);
    void SetCurrentFloor(int32 Floor);
    void SetPortalActivated(bool Activated);


protected:
    // 현재 방의 타입
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
    ERoomType CurrentRoomType;

    // 현재 방의 상태
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
    ERoomState CurrentRoomState;

    // 현재 몇번째 방인지
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
    int32 CurrentRoomIndex;

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


};
