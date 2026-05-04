// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Room/RoomTypes.h"
#include "GunFireGameMode.generated.h"

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

    // 현재 선택된 방 처리
    UFUNCTION(BlueprintCallable, Category = "Room")
    void StartRoom(ERoomType RoomType);

    UFUNCTION(BlueprintCallable, Category = "Room")
    void EndRoom();

protected:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Room")
    ERoomType InitialRoomType;

    // 다음 방 넘어가는 포탈 활성화 함수
    UFUNCTION(BlueprintCallable, Category = "Room")
    void ActivatePortal();

    // 현재 레벨이 어떤 방인지 알아내는 함수
    ARoomBase* FindRoom() const;
};

