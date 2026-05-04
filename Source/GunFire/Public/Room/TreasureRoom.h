#pragma once

#include "CoreMinimal.h"
#include "Room/RoomBase.h"
#include "TreasureRoom.generated.h"


UCLASS()
class GUNFIRE_API ATreasureRoom : public ARoomBase
{
    GENERATED_BODY()

public:
    virtual void OnStart(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState) override;
    virtual void OnEnd(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState) override;
};
