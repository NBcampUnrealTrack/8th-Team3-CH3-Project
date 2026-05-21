#include "Room/CombatRoom.h"

#include "Components/BoxComponent.h"
#include "Game/GunFireGameInstance.h"
#include "Game/GunFireGameState.h"
#include "Enemy/EnemyBase.h"
#include "GunFire/GunFireGameMode.h"
#include "Room/DoorBase.h"

ACombatRoom::ACombatRoom()
{
    PrepareTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("PrepareTrigger"));
    PrepareTrigger->SetupAttachment(Scene);
    PrepareTrigger->SetCollisionProfileName(TEXT("Trigger"));
    PrepareTrigger->SetBoxExtent(FVector(600.f, 600.f, 150.f));
    PrepareTrigger->OnComponentBeginOverlap.AddDynamic(this, &ACombatRoom::OnPrepareTriggerBeginOverlap);

    RoomType = ERoomType::Combat;
    Initialize();
}

// 초기화 함수
void ACombatRoom::Initialize()
{
    RemainingEnemyCount = 0;
    Enemies.Empty();
}

void ACombatRoom::KillEnemyForTest()
{
    for (int32 i = Enemies.Num() - 1; i >= 0; --i)
    {
        if (IsValid(Enemies[i]))
        {
            Enemies[i]->Die();
        }
    }
}

void ACombatRoom::CompleteSelectReward()
{
    if (AGunFireGameMode* GFGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<AGunFireGameMode>() : nullptr)
    {
        GFGameMode->EndCurrentRoom();
    }
}

void ACombatRoom::OnPrepare(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState)
{
    Initialize();

    UE_LOG(LogTemp, Log, TEXT("준비 단계 : 몬스터 스폰 처리"));

    SpawnEnemies();
    GFGameState->SetRemainingEnemyCount(RemainingEnemyCount);

    // 에러나 실수로 0마리가 소환됐다면 로그
    if (RemainingEnemyCount <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Error : 적 스폰 실패!!"));
    }
}

void ACombatRoom::OnEnd(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState)
{
    // 몬스터 정리
    RemainingEnemyCount = 0;

    // 혹시 남아있다면 제거
    for (const auto& Enemy : Enemies)
    {
        if (IsValid(Enemy))
        {
            Enemy->OnEnemyDead.RemoveDynamic(this, &ACombatRoom::HandleEnemyDead);
            Enemy->Destroy();
            UE_LOG(LogTemp, Warning, TEXT("Enemy Destroyed!"));
        }
    }
    Enemies.Empty();
    GFGameState->SetRemainingEnemyCount(0);

    for (const auto& Door : RestrictDoors)
    {
        if (IsValid(Door))
        {
            Door->OpenDoor();
        }
    }
}

void ACombatRoom::PrepareRoom(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState)
{
    if (!GFGameMode || !GFGameState) return;
    if (!IsWaiting()) return;

    for (const auto& Door : RestrictDoors)
    {
        if (IsValid(Door))
        {
            Door->CloseDoor();
        }
    }

    RoomState = ERoomState::Prepared;
    OnPrepare(GFGameMode, GFGameState);
}

// 적 사망 시 호출할 함수
void ACombatRoom::HandleEnemyDead(AEnemyBase* DeadEnemy)
{
    if (!IsValid(DeadEnemy)) return;

    // 바인딩 삭제 및 배열에서 삭제
    // 중복 호출되어 삭제 실패했다면 아래 작업 X
    DeadEnemy->OnEnemyDead.RemoveDynamic(this, &ACombatRoom::HandleEnemyDead);
    if (Enemies.Remove(DeadEnemy) == 0)
    {
        return;
    }

    --RemainingEnemyCount;

    const UWorld* World = GetWorld();
    if (!World) return;

    // GameState 동기화
    if (AGunFireGameState* GameState = World->GetGameState<AGunFireGameState>())
    {
        GameState->SetRemainingEnemyCount(RemainingEnemyCount);
    }

    // 게임 인스턴스에 킬 카운트 추가
    if (UGunFireGameInstance* GameInstance = GetGameInstance<UGunFireGameInstance>())
    {
        GameInstance->AddKilledEnemyCount(1);
    }

    // 적을 전부 처치하면 GameMode에서 현재 방 끝내기
    if (RemainingEnemyCount <= 0)
    {
        OnClearedCombat();
    }
}

void ACombatRoom::OnPrepareTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 플레이어와 충돌했다면
    if (IsValid(OtherActor) && OtherActor->ActorHasTag(TEXT("Player")))
    {
        if (AGunFireGameMode* GFGameMode = GetWorld() ?
                 GetWorld()->GetAuthGameMode<AGunFireGameMode>() :
                 nullptr)
        {
            // 게임모드에서 현재 방 준비
            GFGameMode->TryPrepareRoom(this);
        }
    }
}

void ACombatRoom::OnClearedCombat()
{
    StartSelectReward();
}

// 보상 선택 함수의 cpp 기본 동작
void ACombatRoom::StartSelectReward_Implementation()
{
    // BP에서 유물 선택 / UI 처리
    // 선택이 끝나면 CompleteSelectReward()
    const UEnum* EnumPtr = StaticEnum<ERoomType>();
    if (EnumPtr)
    {
        UE_LOG(LogTemp, Warning, TEXT("RoomType: %s"),
            *EnumPtr->GetDisplayNameTextByValue(static_cast<int64>(RoomType)).ToString());
    }

    UE_LOG(LogTemp, Error, TEXT("유물 선택 즉시 종료, 유물 시스템 연결 후 해당 구문 제거 필요"));
    CompleteSelectReward();
}

void ACombatRoom::SpawnEnemies()
{
    UWorld* World = GetWorld();
    if (!World) return;

    for (const FEnemySpawnGroup& SpawnGroup : EnemySpawnGroups)
    {
        if (!SpawnGroup.EnemyClass) continue;

        for (const TObjectPtr<AActor>& SpawnPointActor : SpawnGroup.SpawnPoints)
        {
            AActor* SpawnPoint = SpawnPointActor.Get();
            if (!IsValid(SpawnPoint)) continue;

            AEnemyBase* SpawnedEnemy = World->SpawnActor<AEnemyBase>(
                SpawnGroup.EnemyClass,
                SpawnPoint->GetActorTransform()
                );

            if (IsValid(SpawnedEnemy))
            {
                SpawnedEnemy->OnEnemyDead.AddDynamic(this, &ACombatRoom::HandleEnemyDead);
                Enemies.Add(SpawnedEnemy);
            }
        }
    }

    RemainingEnemyCount = Enemies.Num();
}
