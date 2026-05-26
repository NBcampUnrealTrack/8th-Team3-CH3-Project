// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GunFireGameMode.generated.h"

class ALevelLayoutManager;
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

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
    TArray<TObjectPtr<USoundBase>> FloorBGMs;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
    float FloorBGMFadeInTime;

protected:
    // 다음 방 넘어가는 포탈 활성화 함수
    UFUNCTION(BlueprintCallable, Category = "Room")
    void ActivatePortal();

    // 레벨 레이아웃 로딩이 완료되면 호출하는 함수
    UFUNCTION()
    void HandleLayoutReady();

    // 로딩 진행도 변화 이벤트에 바인딩하는 함수
    UFUNCTION()
    void HandleLoadingProgress(float Progress);

private:
    bool CanEnterRoom(const ARoomBase* EnteredRoom);
    AStartRoom* FindStartRoom();
    int32 CountCombatRooms();

    // 게임 인스턴스 저장/복구
    void SaveSessionData();
    void RestoreSessionData();

    // 배치된 레벨 레이아웃 매니저 찾는 함수
    ALevelLayoutManager* FindLevelLayoutManager();

    // 로딩 스크린 보여주기
    void ShowLoadingScreen();
    // 로딩 스크린 가리기
    void HideLoadingScreen();
    // 플레이어 행동 막기
    void LockPlayer();
    // 플레이어 행동 풀기
    void UnlockPlayer();

    void PlayCurrentFloorBGM();

private:
    float CurrentGravityScale;
};

