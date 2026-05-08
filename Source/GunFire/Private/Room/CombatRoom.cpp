#include "Room/CombatRoom.h"

#include "Game/GunFireGameInstance.h"
#include "Game/GunFireGameState.h"
#include "Enemy/EnemyBase.h"
#include "GunFire/GunFireGameMode.h"

ACombatRoom::ACombatRoom()
{
    RoomType = ERoomType::Combat;
    SpawningEnemyCount = 1;
    Initialize();
}

void ACombatRoom::KillEnemyForTest()
{
    for (const auto& Enemy : Enemies)
    {
        if (IsValid(Enemy))
        {
            Enemy->Die();
            return;
        }
    }
}

// 초기화 함수
void ACombatRoom::Initialize()
{
    RemainingEnemyCount = 0;
    Enemies.Empty();
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
        UE_LOG(LogTemp, Error, TEXT("Error : Enemy Not Spawned!!"));
        GFGameMode->EndCurrentRoom();
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

    UE_LOG(LogTemp, Warning, TEXT("Combat Room End"));
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
        if (AGunFireGameMode* GameMode = World->GetAuthGameMode<AGunFireGameMode>())
        {
            GameMode->EndCurrentRoom();
        }
    }
}

void ACombatRoom::SpawnEnemies()
{
    UWorld* World = GetWorld();
    if (!World) return;

    if (EnemyClasses.IsEmpty()) return;

    for (int32 i = 0; i < SpawningEnemyCount; ++i)
    {
        // 스폰 포인트 확인
        if (i >= EnemySpawnPoints.Num()) break;

        AActor* SpawnPoint = EnemySpawnPoints[i].Get();
        if (!SpawnPoint)
        {
            UE_LOG(LogTemp, Warning, TEXT("스폰 포인트 찾기 실패!"));
            continue;
        }

        // 소환할 적 클래스 확인
        TSubclassOf<AEnemyBase> EnemyClass = EnemyClasses[i % EnemyClasses.Num()];
        if (!EnemyClass)
        {
            UE_LOG(LogTemp, Warning, TEXT("지정한 적 클래스가 없습니다!"));
            continue;
        }

        AEnemyBase* SpawnedEnemy = World->SpawnActor<AEnemyBase>(
            EnemyClass,
            SpawnPoint->GetActorTransform()
            );

        // 생성한 적 컨테이너에 추가
        if (SpawnedEnemy)
        {
            SpawnedEnemy->OnEnemyDead.AddDynamic(this, &ACombatRoom::HandleEnemyDead);
            Enemies.Add(SpawnedEnemy);
        }
    }

    RemainingEnemyCount = Enemies.Num();
}
