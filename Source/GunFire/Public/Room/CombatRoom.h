#pragma once

#include "CoreMinimal.h"
#include "Room/RoomBase.h"
#include "CombatRoom.generated.h"

// class AEnemyBase; 

UCLASS()
class GUNFIRE_API ACombatRoom : public ARoomBase
{
    GENERATED_BODY()

public:
    ACombatRoom();

protected:
    virtual void OnStart(class AGunFireGameMode* GFGameMode, class AGunFireGameState* GFGameState) override;
    virtual void OnEnd(class AGunFireGameMode* GFGameMode, class AGunFireGameState* GFGameState) override;


    // UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room|Combat")
    // TArray<TSubclassOf<class AEnemyBase>> EnemyClasses;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room|Combat")
    TArray<TObjectPtr<AActor>> EnemySpawnPoints;

    // UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Room|Combat")
    // TArray<TObjectPtr<class AEnemyBase>> Enemies;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room|Combat")
    int32 SpawningEnemyCount;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Room|Combat")
    int32 RemainingEnemyCount;

    // UFUNCTION()
    // void HandleEnemyDead(class AEnemyBase* DeadEnemy);

    void Initialize();

private:
    void SpawnEnemies();
};
