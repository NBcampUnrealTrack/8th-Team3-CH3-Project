#include "Room/RandomRoom.h"

#include "Game/GunFireGameState.h"
#include "GunFire/GunFireGameMode.h"

ARandomRoom::ARandomRoom()
{
    RoomType = ERoomType::Random;
    RoomMode = ERandomRoomMode::Relic;
}

void ARandomRoom::OnPrepare(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState)
{
    RoomMode = DecideRoomMode();

    if (RoomMode == ERandomRoomMode::EliteCombat)
    {
        // CombatRoom의 전투 로직 그대로 사용
        Super::OnPrepare(GFGameMode, GFGameState);
        return;
    }

    Initialize();
    GFGameState->SetRemainingEnemyCount(0);
    UE_LOG(LogTemp, Warning, TEXT("랜덤방, 유물 준비"));
}

void ARandomRoom::OnStart(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState)
{
    if (RoomMode == ERandomRoomMode::Relic)
    {
        StartSelectReward();
        return;
    }

    // 엘리트 전투는 전투 가능하게 부모 OnStart 호출
    Super::OnStart(GFGameMode, GFGameState);
}

void ARandomRoom::OnClearedCombat()
{
    // CombatRoom의 보상 선택 함수를 호출해서 방을 즉시 종료함
    // 강화 전투방은 전투 로직은 사용하지만 보상이 없게 됨
    UE_LOG(LogTemp, Warning, TEXT("랜덤방 : 강화 전투 종료"));
    CompleteSelectReward();
}

ERandomRoomMode ARandomRoom::DecideRoomMode()
{
    ERandomRoomMode RandomMode = FMath::RandBool()
        ? ERandomRoomMode::Relic
        : ERandomRoomMode::EliteCombat;

    // 유물방이면 검사
    if (RandomMode == ERandomRoomMode::Relic)
    {
        if (AGunFireGameMode* GFGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<AGunFireGameMode>() : nullptr)
        {
            // 유물방을 생성할 수 있는지 판단해서 가능하면 유물방으로 생성
            if (GFGameMode->TryGenerateRandomRelicRoom())
            {
                return ERandomRoomMode::Relic;
            }
        }
    }
    return ERandomRoomMode::EliteCombat;
}

