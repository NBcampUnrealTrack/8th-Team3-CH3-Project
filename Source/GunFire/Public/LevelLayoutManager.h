#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "LevelLayoutManager.generated.h"

UCLASS()
class GUNFIRE_API ALevelLayoutManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ALevelLayoutManager();

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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
    void SpawnRoomsAtSlots(
        const TArray<TSoftObjectPtr<UWorld>>& Pool,
        FName SlotTag);
    void SpawnCorridors();
    void SpawnCorridorBetween(AActor* A, AActor* B);
};
