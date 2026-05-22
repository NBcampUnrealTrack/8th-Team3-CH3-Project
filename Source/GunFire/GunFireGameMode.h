// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GunFireGameMode.generated.h"

enum class ESessionResult : uint8;
class AStartRoom;
class AGunFireGameState;
class ARoomBase;
class ACombatRoom;

UCLASS(minimalapi)
class AGunFireGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGunFireGameMode();

    // 게임 모드 기본 함수
    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
    virtual void StartPlay() override;

    UFUNCTION(BlueprintCallable, Category = "Room")
    void EnterStartRoom();

    UFUNCTION(BlueprintCallable, Category = "Room")
    void EndStartRoom();

    UFUNCTION(BlueprintCallable, Category = "Room")
    void TryPrepareRoom(ACombatRoom* EnteredRoom);

    UFUNCTION(BlueprintCallable, Category = "Room")
    void TryEnterRoom(ARoomBase* EnteredRoom);

    UFUNCTION(BlueprintCallable, Category = "Room")
    void StartCurrentRoom();

    UFUNCTION(BlueprintCallable, Category = "Room")
    void EndCurrentRoom();

    UFUNCTION(BlueprintCallable, Category = "Room")
    void TryEnterNextFloor(FName NextLevelName);

    UFUNCTION(BlueprintPure, Category = "Room")
    bool IsCurrentRoom(ARoomBase* Room) const;

    UFUNCTION(BlueprintCallable, Category = "Game")
    void ClearGame();

    UFUNCTION(BlueprintCallable, Category = "Game")
    void GameOver();

    UFUNCTION(BlueprintCallable, Category = "Game")
    bool TryGenerateRandomRelicRoom();

    // 디버그용 적을 처치하는 함수
    UFUNCTION( BlueprintCallable, Category = "Room|Debug")
    void KillEnemyForTest();

protected:
    // 결과창 레벨의 이름
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Room")
    FName ResultLevelName;

    // 현재 방을 가리키는 포인터
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room")
    TObjectPtr<ARoomBase> CurrentRoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room")
    TObjectPtr<AStartRoom> StartingRoom;

    // 다음 층으로 이동하는데 필요한 전투방 클리어 횟수
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room")
    int32 RequiredCombatRoomCount;

    // 현재 클리어한 전투방 횟수
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room")
    int32 ClearedCombatRoomCount;

    // 한층에 랜덤방에서 유물방이 등장할 수 있는 최대 횟수
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room")
    int32 MaxRandomRelicRoomCount;

    // 현재 랜덤방에서 유물방이 등장한 횟수
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room")
    int32 CurrentRandomRelicRoomCount;

protected:
    // 다음 방 넘어가는 포탈 활성화 함수
    UFUNCTION(BlueprintCallable, Category = "Room")
    void ActivatePortal();

private:
    bool CanEnterRoom(const ARoomBase* EnteredRoom);
    AStartRoom* FindStartRoom();
    int32 CountCombatRooms();
    void GoToResultLevel();

    void SaveSessionData();
    void RestoreSessionData();
};

