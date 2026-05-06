#pragma once

#include "CoreMinimal.h"
#include "Room/RoomBase.h"
#include "CombatRoom.generated.h"


UCLASS()
class GUNFIRE_API ACombatRoom : public ARoomBase
{
	GENERATED_BODY()

public:
    ACombatRoom();

protected:
    virtual void OnStart(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState) override;
    virtual void OnEnd(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState) override;
};
