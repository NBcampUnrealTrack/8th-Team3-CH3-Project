#include "Room/CombatRoom.h"

#include "Game/GunFireGameInstance.h"
#include "Game/GunFireGameState.h"
// #include "EnemyBase.h" 
#include "GunFire/GunFireGameMode.h"

ACombatRoom::ACombatRoom()
{
    RoomType = ERoomType::Combat;
    SpawningEnemyCount = 1;
    Initialize();
}

// 초기화 함수
void ACombatRoom::Initialize()
{
    RemainingEnemyCount = 0;
    // Enemies.Empty(); 
}

void ACombatRoom::OnStart(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState)
{
    Initialize();

    UE_LOG(LogTemp, Warning, TEXT("Combat Room Start"));

    SpawnEnemies();
    GFGameState->SetRemainingEnemyCount(RemainingEnemyCount);

    // 에러나 실수로 0마리가 소환됐다면 방을 종료하고 로그
    if (RemainingEnemyCount <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Error : Enemy Not Spawned!! (Current Branch: No Enemy Class)"));

    }
}

void ACombatRoom::OnEnd(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState)
{
    RemainingEnemyCount = 0;

    /* 
    for (const auto& Enemy : Enemies)
    {
        if (IsValid(Enemy))
        {
            // Enemy->OnEnemyDead.RemoveDynamic(this, &ACombatRoom::HandleEnemyDead);
            Enemy->Destroy();
            UE_LOG(LogTemp, Warning, TEXT("Enemy Destroyed!"));
        }
    }
    Enemies.Empty();
    */
    GFGameState->SetRemainingEnemyCount(0);

    UE_LOG(LogTemp, Warning, TEXT("Combat Room End"));
}

/* 
void ACombatRoom::HandleEnemyDead(AEnemyBase* DeadEnemy)
{
    if (!IsValid(DeadEnemy)) return;

    // DeadEnemy->OnEnemyDead.RemoveDynamic(this, &ACombatRoom::HandleEnemyDead);
    // Enemies.Remove(DeadEnemy);

    --RemainingEnemyCount;

    const UWorld* World = GetWorld();
    if (!World) return;

    if (AGunFireGameState* GameState = World->GetGameState<AGunFireGameState>())
    {
        GameState->SetRemainingEnemyCount(RemainingEnemyCount);
    }

    if (UGunFireGameInstance* GameInstance = GetGameInstance<UGunFireGameInstance>())
    {
        GameInstance->AddKilledEnemyCount(1);
    }

    if (RemainingEnemyCount <= 0)
    {
        if (AGunFireGameMode* GameMode = World->GetAuthGameMode<AGunFireGameMode>())
        {
            GameMode->EndCurrentRoom();
        }
    }
}
*/

void ACombatRoom::SpawnEnemies()
{
    UWorld* World = GetWorld();
    if (!World) return;

    /*
    if (EnemyClasses.IsEmpty()) return;

    for (int32 i = 0; i < SpawningEnemyCount; ++i)
    {
        if (i >= EnemySpawnPoints.Num()) break;

        AActor* SpawnPoint = EnemySpawnPoints[i].Get();
        if (!SpawnPoint) continue;

        // AEnemyBase 기반 스폰 로직 주석 처리
    }
    */

    RemainingEnemyCount = 0; // 현재는 0으로 고정
}
