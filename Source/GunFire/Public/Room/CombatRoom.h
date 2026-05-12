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

    void Initialize();

    // 테스트용 적 한마리 처치하는 함수
    UFUNCTION(BlueprintCallable, Category = "Room|Combat|Debug")
    void KillEnemyForTest();

    // 보상(유물) 선택시 현재 방을 끝내는 함수
    UFUNCTION(BlueprintCallable)
    void CompleteSelectReward();

protected:
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

protected:
    virtual void OnStart(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState) override;
    virtual void OnEnd(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState) override;

    // 전투가 끝났을 때 호출됨, 기본 CombatRoom은 보상 선택으로 넘어감
    virtual void OnClearedCombat();

    // 보상 선택, BP에서 보상 선택 UI 처리
    UFUNCTION(BlueprintNativeEvent, Category = "Room|Combat|Reward")
    void StartSelectReward();

    UFUNCTION()
    void HandleEnemyDead(AEnemyBase* DeadEnemy);

private:
    void SpawnEnemies();
};
