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
    ERoomType GetRoomType() const;
    UFUNCTION(BlueprintPure)
    ERoomState GetRoomState() const;
    UFUNCTION(BlueprintPure)
    int32 GetRemainingEnemyCount() const;
    UFUNCTION(BlueprintPure)
    bool GetPortalActivated() const;

    void SetRoomType(ERoomType Type);
    void SetRoomState(ERoomState State);
    void SetRemainingEnemyCount(int32 Count);
    void SetPortalActivated(bool Activated);


protected:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
    ERoomType CurrentRoomType;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
    ERoomState CurrentRoomState;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
    int32 RemainingEnemyCount;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
    bool bPortalActivated;
};
