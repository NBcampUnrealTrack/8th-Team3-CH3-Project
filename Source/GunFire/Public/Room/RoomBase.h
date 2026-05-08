#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Room/RoomTypes.h"
#include "RoomBase.generated.h"

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

    bool CanMoveNextRoom(const ARoomBase* NextRoom) const;
    bool HasNextRooms() const;
    void UnlockNextRooms();

    ERoomType GetRoomType() const;
    FName GetRoomID() const;
    bool IsStarted() const;
    bool IsCleared() const;
    bool IsLocked() const;

    void SetLocked(bool bNewLocked);

protected:
    // 씬 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room|Component")
    TObjectPtr<USceneComponent> Scene;

    // 진입 확인용 트리거 박스
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room|Component")
    TObjectPtr<UBoxComponent> EntryTrigger;

    // 방의 ID
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room")
    FName RoomID;

    // 방 타입
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room")
    ERoomType RoomType;

    // 다음 방
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room")
    TArray<TObjectPtr<ARoomBase>> NextRooms;

    // 방 시작 했는지
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room")
    bool bStarted;

    // 방 클리어 했는지
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room")
    bool bCleared;

    // 방이 닫혀있는지
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room")
    bool bLocked;

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
