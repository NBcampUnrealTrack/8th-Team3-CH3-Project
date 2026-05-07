#pragma once

#include "CoreMinimal.h"
#include "Room/RoomBase.h"
#include "CombatRoom.generated.h"


class AEnemyBase;

UCLASS()
class GUNFIRE_API ACombatRoom : public ARoomBase
{
	GENERATED_BODY()

public:
    ACombatRoom();

    // 테스트용 적 한마리 처치하는 함수
    UFUNCTION(BlueprintCallable, Category = "Room|Combat|Debug")
    void KillEnemyForTest();

protected:
    virtual void OnStart(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState) override;
    virtual void OnEnd(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState) override;

    // 생성할 적 클래스 목록
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room|Combat")
    TArray<TSubclassOf<AEnemyBase>> EnemyClasses;

    // 스폰 포인트 지정
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room|Combat")
    TArray<TObjectPtr<AActor>> EnemySpawnPoints;

    // 생성된 적들을 보관하는 컨테이너
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Room|Combat")
    TArray<TObjectPtr<AEnemyBase>> Enemies;

    // 소환할 적의 수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room|Combat")
    int32 SpawningEnemyCount;

    // 남아있는 적의 수
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Room|Combat")
    int32 RemainingEnemyCount;


    UFUNCTION()
    void HandleEnemyDead(AEnemyBase* DeadEnemy);

    void Initialize();

private:
    void SpawnEnemies();
};
