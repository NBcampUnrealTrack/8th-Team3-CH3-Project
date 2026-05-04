#pragma once

#include "CoreMinimal.h"
#include "Room/RoomBase.h"
#include "TrapRoom.generated.h"


UCLASS()
class GUNFIRE_API ATrapRoom : public ARoomBase
{
    GENERATED_BODY()

public:
    virtual void OnStart(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState) override;
    virtual void OnEnd(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState) override;
};
