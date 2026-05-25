#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "LevelLayoutManager.generated.h"

class ULevelStreamingDynamic;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLayoutReadySignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadingProgressSignature, float, Progress);

UCLASS()
class GUNFIRE_API ALevelLayoutManager : public AActor
{
	GENERATED_BODY()

public:
	ALevelLayoutManager();

    // 레벨들을 로드하는 함수
    UFUNCTION(BlueprintCallable, Category = "Layout")
    void StartLevel();


    UPROPERTY(EditAnywhere, Category = "Layout")
    TArray<TSoftObjectPtr<UWorld>> StartRoomPool;

    UPROPERTY(EditAnywhere, Category = "Layout")
    FName StartSlotTag = TEXT("StartSlot");

    UPROPERTY(EditAnywhere, Category = "Layout")
    TArray<TSoftObjectPtr<UWorld>> BattleRoomPool;

    UPROPERTY(EditAnywhere, Category = "Layout")
    FName BattleSlotTag = TEXT("BattleSlot");

    UPROPERTY(EditAnywhere, Category = "Layout")
    TArray<TSoftObjectPtr<UWorld>> RandomRoomPool;

    UPROPERTY(EditAnywhere, Category = "Layout")
    FName RandomSlotTag = TEXT("RandomSlot");

    UPROPERTY(EditAnywhere, Category = "Layout")
    TArray<TSoftObjectPtr<UWorld>> BossRoomPool;

    UPROPERTY(EditAnywhere, Category = "Layout")
    FName BossSlotTag = TEXT("BossSlot");

    UPROPERTY(EditAnywhere, Category = "Layout")
    TSoftObjectPtr<UWorld> CorridorAsset;

    UPROPERTY(EditAnywhere, Category = "Layout")
    float MaxCorridorDistance = 8000.f;

    // 로딩 완료를 알리는 델리게이트
    UPROPERTY(BlueprintAssignable)
    FOnLayoutReadySignature OnLayoutReady;

    // 로딩 진행도를 알리는 델리게이트
    UPROPERTY(BlueprintAssignable)
    FOnLoadingProgressSignature OnLoadingProgress;

protected:

    void CheckStreamingLevel(ULevelStreamingDynamic* StreamingLevel, bool bSuccess);

    // 스트리밍 레벨 로드 완료 시 호출하는 함수
    UFUNCTION()
    void HandleStreamingLevelShown();

protected:
    // LoadLevelInstanceBySoftObjectPtr의 반환값들을 저장하는 배열
    // 스트리밍 레벨들을 추적하기 위함
    UPROPERTY()
    TArray<TObjectPtr<ULevelStreamingDynamic>> StreamingLevels;

    // 로드 시도한 스트리밍 레벨 개수
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Layout")
    int32 ExpectedStreamingLevelCount;

    // 완료된 스트리밍 레벨 개수
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Layout")
    int32 ReadiedStreamingLevelCount;

    bool bStartLoading;
    bool bEndLoading;

private:
    void SpawnRoomsAtSlots(
        const TArray<TSoftObjectPtr<UWorld>>& Pool,
        FName SlotTag);
    void SpawnCorridors();
    void SpawnCorridorBetween(AActor* A, AActor* B);

    // 모든 로딩이 끝났는지 확인하는 함수
    void CheckLayoutReady();
};
