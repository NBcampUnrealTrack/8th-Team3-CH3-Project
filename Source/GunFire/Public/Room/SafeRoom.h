#pragma once

#include "CoreMinimal.h"
#include "Room/RoomBase.h"
#include "SafeRoom.generated.h"


UCLASS()
class GUNFIRE_API ASafeRoom : public ARoomBase
{
    GENERATED_BODY()

public:
    ASafeRoom();

protected:
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Room|Component")
    TObjectPtr<UBoxComponent> ExitTrigger;

    virtual void OnStart(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState) override;
    virtual void OnEnd(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState) override;

    UFUNCTION()
    void OnExitTriggerBeginOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
        );
};
