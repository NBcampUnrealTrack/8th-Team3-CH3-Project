#pragma once

#include "CoreMinimal.h"
#include "Room/RoomBase.h"
#include "CombatRoom.generated.h"


class AEnemyBase;
class UBoxComponent;
class ADoorBase;

UCLASS()
class GUNFIRE_API ACombatRoom : public ARoomBase
{
	GENERATED_BODY()

public:
    ACombatRoom();

    void Initialize();

    // 방을 준비하는 함수, 스폰처리 및 State 변경
    void PrepareRoom(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState);

    // 테스트용 적 한마리 처치하는 함수
    UFUNCTION(BlueprintCallable, Category = "Room|Combat|Debug")
    void KillEnemyForTest();

    // 보상(유물) 선택시 현재 방을 끝내는 함수
    UFUNCTION(BlueprintCallable)
    void CompleteSelectReward();

protected:
    // 준비용 트리거박스, 몬스터 스폰
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room|Component")
    TObjectPtr<UBoxComponent> PrepareTrigger;

    // 생성할 적 클래스 목록
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room|Combat")
    TArray<TSubclassOf<AEnemyBase>> EnemyClasses;

    // 스폰 포인트 지정
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room|Combat")
    TArray<TObjectPtr<AActor>> EnemySpawnPoints;

    // 생성된 적들을 보관하는 컨테이너
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Room|Combat")
    TArray<TObjectPtr<AEnemyBase>> Enemies;

    // Prepare Trigger 발동 시 뒤로 못돌아가게 막는 문
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room")
    TArray<TObjectPtr<ADoorBase>> RestrictDoors;

    // 소환할 적의 수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room|Combat")
    int32 SpawningEnemyCount;

    // 남아있는 적의 수
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Room|Combat")
    int32 RemainingEnemyCount;

protected:
    // 실제로 몬스터 스폰이 일어나는 함수
    virtual void OnPrepare(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState);
    // 남아있는 몬스터 정리, 방 정리하는 함수
    virtual void OnEnd(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState) override;

    // 전투가 끝났을 때 호출됨, 기본 CombatRoom은 보상 선택으로 넘어감
    virtual void OnClearedCombat();

    // 보상 선택, BP에서 보상 선택 UI 처리
    UFUNCTION(BlueprintNativeEvent, Category = "Room|Combat|Reward")
    void StartSelectReward();

    // 몬스터 사망시 호출되는 이벤트
    UFUNCTION()
    void HandleEnemyDead(AEnemyBase* DeadEnemy);

    // PrePare 트리거의 오버랩 이벤트 함수
    UFUNCTION()
    void OnPrepareTriggerBeginOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
        );

private:
    void SpawnEnemies();
};
