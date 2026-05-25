#include "LevelLayoutManager.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "Algo/RandomShuffle.h"


ALevelLayoutManager::ALevelLayoutManager()
{
	PrimaryActorTick.bCanEverTick = false;

    ExpectedStreamingLevelCount = 0;
    ReadiedStreamingLevelCount = 0;
    StreamingLevels.Empty();
    bStartLoading = false;
    bEndLoading = false;
}

void ALevelLayoutManager::StartLevel()
{
    if (bStartLoading) return;

    bStartLoading = true;
    ExpectedStreamingLevelCount = 0;
    ReadiedStreamingLevelCount = 0;
    StreamingLevels.Empty();

    OnLoadingProgress.Broadcast(0.f);

    SpawnRoomsAtSlots(StartRoomPool, StartSlotTag);
    SpawnRoomsAtSlots(BattleRoomPool, BattleSlotTag);
    SpawnRoomsAtSlots(RandomRoomPool, RandomSlotTag);
    SpawnRoomsAtSlots(BossRoomPool, BossSlotTag);
    SpawnCorridors();

    CheckLayoutReady();
}

void ALevelLayoutManager::CheckStreamingLevel(ULevelStreamingDynamic* StreamingLevel, bool bSuccess)
{
    if (!bSuccess || !IsValid(StreamingLevel)) return;

    ++ExpectedStreamingLevelCount;
    StreamingLevels.Add(StreamingLevel);

    StreamingLevel->OnLevelShown.AddDynamic(this, &ALevelLayoutManager::HandleStreamingLevelShown);
}

void ALevelLayoutManager::HandleStreamingLevelShown()
{
    ++ReadiedStreamingLevelCount;

    float Progress = ExpectedStreamingLevelCount > 0
        ? static_cast<float>(ReadiedStreamingLevelCount) / static_cast<float>(ExpectedStreamingLevelCount)
        : 1.f;

    OnLoadingProgress.Broadcast(Progress);

    CheckLayoutReady();
}

void ALevelLayoutManager::SpawnRoomsAtSlots(
    const TArray<TSoftObjectPtr<UWorld>>& Pool,
    FName SlotTag)
{
    TArray<AActor*> FoundSlots;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), SlotTag, FoundSlots);

    if (FoundSlots.Num() == 0 && Pool.Num() == 0)
    {
        return;
    }

    if (FoundSlots.Num() == 0)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[LayoutManager] Pool has entries but no slots tagged: %s"),
            *SlotTag.ToString());
        return;
    }

    if (Pool.Num() < FoundSlots.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("[LayoutManager] Pool %d < slots %d (%s)"),
            Pool.Num(), FoundSlots.Num(), *SlotTag.ToString());
        return;
    }

    TArray<TSoftObjectPtr<UWorld>> ShuffledPool = Pool;
    Algo::RandomShuffle(ShuffledPool);

    for (int32 i = 0; i < FoundSlots.Num(); ++i)
    {
        AActor* Slot = FoundSlots[i];
        bool bSuccess = false;
        ULevelStreamingDynamic* StreamingLevel = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(
            this, ShuffledPool[i],
            Slot->GetActorLocation(),
            Slot->GetActorRotation(),
            bSuccess);

        CheckStreamingLevel(StreamingLevel, bSuccess);
    }
}

void ALevelLayoutManager::SpawnCorridors()
{
    TArray<AActor*> StartSlots, BattleSlots, RandomSlots, BossSlots;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), StartSlotTag, StartSlots);   // 하드코딩 제거
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), BattleSlotTag, BattleSlots);
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), RandomSlotTag, RandomSlots);
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), BossSlotTag, BossSlots);     // 추가

    for (AActor* Start : StartSlots)
        for (AActor* Battle : BattleSlots)
            SpawnCorridorBetween(Start, Battle);

    for (AActor* Battle : BattleSlots)
        for (AActor* Random : RandomSlots)
        {
            float Dist = FVector::Dist(
                Battle->GetActorLocation(),
                Random->GetActorLocation());

            UE_LOG(LogTemp, Warning,
                TEXT("[Corridor] Battle %s ↔ Random %s: Dist=%.1f"),
                *Battle->GetName(),
                *Random->GetName(),
                Dist);

            if (Dist < MaxCorridorDistance)
                SpawnCorridorBetween(Battle, Random);
        }

    for (AActor* Start : StartSlots)
        for (AActor* Boss : BossSlots)
            SpawnCorridorBetween(Start, Boss);
}

void ALevelLayoutManager::SpawnCorridorBetween(AActor* A, AActor* B)
{
    if (CorridorAsset.IsNull()) return;

    FVector PosA = A->GetActorLocation();
    FVector PosB = B->GetActorLocation();
    FVector Mid = (PosA + PosB) * 0.5f;
    FRotator Rot = (PosB - PosA).Rotation();
    Rot.Pitch = 0; Rot.Roll = 0; // Yaw만 사용

    bool bSuccess = false;
    ULevelStreamingDynamic* StreamingLevel = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(
        this, CorridorAsset, Mid, Rot, bSuccess);

    CheckStreamingLevel(StreamingLevel, bSuccess);
}

void ALevelLayoutManager::CheckLayoutReady()
{
    if (bEndLoading) return;
    if (ReadiedStreamingLevelCount < ExpectedStreamingLevelCount) return;

    // 로딩 종료 알림
    bEndLoading = true;
    OnLoadingProgress.Broadcast(1.f);
    OnLayoutReady.Broadcast();
}
