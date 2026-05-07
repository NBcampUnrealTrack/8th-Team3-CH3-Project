// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Room/RoomTypes.h"
#include "GunFireGameMode.generated.h"

class ASafeRoom;
class AGunFireGameState;
class ARoomBase;

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
    void EnterInitialSafeRoom();

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

    // 디버그용 적을 처치하는 함수
    UFUNCTION( BlueprintCallable, Category = "Room|Debug")
    void KillEnemyForTest();

protected:
    // 초기 방의 타입
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Room")
    ERoomType InitialRoomType;

    // 맨 처음 휴식방의 ID
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Room")
    FName InitialSafeRoomID;

    // 현재 방을 가리키는 포인터
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room")
    TObjectPtr<ARoomBase> CurrentRoom;

    // 다음 방 넘어가는 포탈 활성화 함수
    UFUNCTION(BlueprintCallable, Category = "Room")
    void ActivatePortal();

private:
    bool CanEnterRoom(const ARoomBase* EnteredRoom);
    ASafeRoom* FindInitialSafeRoom();
};

