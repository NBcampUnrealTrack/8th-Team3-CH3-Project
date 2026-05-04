#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomBase.generated.h"

class AGunFireGameState;
class AGunFireGameMode;

UCLASS()
class GUNFIRE_API ARoomBase : public AActor
{
	GENERATED_BODY()

public:
	ARoomBase();

    virtual void StartRoom(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState);
    virtual void OnStart(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState);
    virtual void EndRoom(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState);
    virtual void OnEnd(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState);
};
