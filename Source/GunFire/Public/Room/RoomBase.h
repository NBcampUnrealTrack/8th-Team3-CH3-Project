#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Room/RoomTypes.h"
#include "RoomBase.generated.h"

class ADoorBase;
class AGunFireGameState;
class AGunFireGameMode;
class UBoxComponent;

UCLASS()
class GUNFIRE_API ARoomBase : public AActor
{
	GENERATED_BODY()

public:
	ARoomBase();

    void StartRoom(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState);
    void EndRoom(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState);

    FName GetRoomID() const;
    ERoomType GetRoomType() const;
    ERoomState GetRoomState() const;

    bool IsWaiting() const;
    bool IsInProgress() const;
    bool IsCleared() const;

protected:
    // 씬 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room|Component")
    TObjectPtr<USceneComponent> Scene;

    // 방 시작용 트리거 박스
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room|Component")
    TObjectPtr<UBoxComponent> StartTrigger;

    // 방의 ID
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room")
    FName RoomID;

    // 방 타입 (시작, 전투, 랜덤, 보스 ...)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room")
    ERoomType RoomType;

    // 방 상태 (Waiting, InProgress, Cleared)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room")
    ERoomState RoomState;

    // 방과 연결된 문들
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room")
    TArray<TObjectPtr<ADoorBase>> ConnectedDoors;


protected:
    // 시작, 종료 시 각각의 방에서 처리해야 할 함수
    virtual void OnStart(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState);
    virtual void OnEnd(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState);

    // 트리거 박스 오버랩 합수
    UFUNCTION()
    void OnEntryTriggerBeginOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
        );
};
