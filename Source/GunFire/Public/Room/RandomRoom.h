#pragma once

#include "CoreMinimal.h"
#include "Room/CombatRoom.h"
#include "RandomRoom.generated.h"

UENUM(BlueprintType)
enum class ERandomRoomMode : uint8
{
    Relic       UMETA(DisplayName = "유물"),
    EliteCombat UMETA(DisplayName = "강화 전투")
};

UCLASS()
class GUNFIRE_API ARandomRoom : public ACombatRoom
{
	GENERATED_BODY()

public:
    ARandomRoom();

protected:
    UPROPERTY(VisibleAnywhere, Category = "Room|Random")
    ERandomRoomMode RoomMode;

protected:
    virtual void OnPrepare(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState) override;
    virtual void OnStart(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState) override;
    virtual void OnClearedCombat() override;

    ERandomRoomMode DecideRoomMode();
};
