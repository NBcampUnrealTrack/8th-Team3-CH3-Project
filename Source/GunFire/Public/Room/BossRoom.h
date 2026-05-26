#pragma once

#include "CoreMinimal.h"
#include "Room/CombatRoom.h"
#include "BossRoom.generated.h"


class APortal;

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
};
