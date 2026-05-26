#pragma once

#include "CoreMinimal.h"
#include "Room/RoomBase.h"
#include "StartRoom.generated.h"


class APortal;
class ADoorBase;

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

    // 무기를 얻었을 때 호출 — 통로 문 4개의 F 상호작용을 푼다.
    // 무기/획득 시스템 또는 BP_StartRoom에서 "첫 무기 획득" 시점에 호출한다.
    // (2층 이상: 무기가 이미 복구되어 있으므로 층 시작 시점에도 한 번 호출 필요 — 아래 .cpp 주석 참고)
    UFUNCTION(BlueprintCallable, Category = "Room|Start")
    void UnlockWeaponGateDoors();

protected:
    virtual void OnStart(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState) override;
    virtual void OnEnd(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState) override;

    // 통로 문 진입 보간이 완료됐을 때 호출 — 그 문을 사라지게 하고 StartRoom을 종료시킨다.
    UFUNCTION()
    void HandleWeaponGateDoorEntered(ADoorBase* Door);

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

    // 방과 방 사이 통로로 이어지는 문들.
    // 처음엔 F 상호작용이 잠겨 있고(BeginPlay), 무기를 얻으면 UnlockWeaponGateDoors로 풀린다.
    // 이 중 하나로 진입을 완료하면 그 문이 사라지고 StartRoom이 종료된다.
    // 주의 : RoomBase의 EntranceDoors와는 별개로 둘 것. (여기 넣은 문은 EntranceDoors에 넣지 않는다)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room|Start")
    TArray<TObjectPtr<ADoorBase>> WeaponGateDoors;
};
