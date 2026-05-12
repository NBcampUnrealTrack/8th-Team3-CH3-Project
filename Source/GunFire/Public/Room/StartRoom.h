#pragma once

#include "CoreMinimal.h"
#include "Room/RoomBase.h"
#include "StartRoom.generated.h"


class APortal;

UCLASS()
class GUNFIRE_API AStartRoom : public ARoomBase
{
    GENERATED_BODY()

public:
    AStartRoom();

    virtual void BeginPlay() override;


    UFUNCTION(BlueprintCallable)
    void ActivateFloorPortal();

    UFUNCTION(BlueprintPure)
    AActor* GetPlayerSpawnPoint() const;

protected:
    virtual void OnStart(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState) override;
    virtual void OnEnd(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState) override;

protected:
    // 층 이동 포탈
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room|Start")
    TObjectPtr<APortal> FloorPortal;

    // 스폰 포인트 지정
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room|Start")
    TObjectPtr<AActor> PlayerSpawnPoint;

    // 즉시 종료할지 판단하는 변수
    // 3층의 경우 시작방에선 즉시 종료해야 함
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room|Start")
    bool bEndImmediately;
};
