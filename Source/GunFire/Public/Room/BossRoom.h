#pragma once

#include "CoreMinimal.h"
#include "Room/CombatRoom.h"
#include "BossRoom.generated.h"


class APortal;
class USoundBase;
class UAudioComponent;

UCLASS()
class GUNFIRE_API ABossRoom : public ACombatRoom
{
	GENERATED_BODY()

public:
    ABossRoom();

    void ActivateResultPortal();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room|Boss")
    TObjectPtr<APortal> ResultPortal;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room|Boss")
    TObjectPtr<USoundBase> BossTheme;

    UPROPERTY(Transient)
    TObjectPtr<UAudioComponent> BossThemeComponent;

    virtual void OnPrepare(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState) override;

    virtual void OnEnd(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState) override;
};
