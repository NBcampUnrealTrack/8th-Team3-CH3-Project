#include "Room/RandomRoom.h"

#include "GunFire/GunFireGameMode.h"

ARandomRoom::ARandomRoom()
{
    RoomType = ERoomType::Random;
    RoomMode = ERandomRoomMode::Relic;
}

void ARandomRoom::OnStart(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState)
{
    RoomMode = DecideRoomMode();

    if (RoomMode == ERandomRoomMode::EliteCombat)
    {
        // CombatRoom의 전투 로직 그대로 사용
        Super::OnStart(GFGameMode, GFGameState);
        return;
    }

    StartSelectReward();
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

