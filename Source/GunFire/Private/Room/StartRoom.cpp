#include "Room/StartRoom.h"

#include "GunFire/GunFireGameMode.h"
#include "Interactables/Portal.h"
#include "Room/DoorBase.h"

AStartRoom::AStartRoom()
{
    RoomType = ERoomType::Start;
    bEndImmediately = false;
}

void AStartRoom::BeginPlay()
{
    Super::BeginPlay();

    if (IsValid(FloorPortal))
    {
        FloorPortal->SetActive(false);
    }

    // 통로 문 초기화 : F 상호작용을 잠그고, 진입 완료 콜백을 바인딩한다.
    // 잠금 상태에서도 문은 보이고 콜리전으로 막혀 있다 — 무기를 얻기 전엔 통과 불가.
    for (const TObjectPtr<ADoorBase>& Door : WeaponGateDoors)
    {
        if (IsValid(Door))
        {
            Door->SetInteractionLocked(true);
            Door->OnDoorEntered.AddDynamic(this, &AStartRoom::HandleWeaponGateDoorEntered);
        }
    }
}

void AStartRoom::ActivateFloorPortal()
{
    if (IsValid(FloorPortal))
    {
        FloorPortal->SetActive(true);
    }
}

AActor* AStartRoom::GetPlayerSpawnPoint() const
{
    return PlayerSpawnPoint.Get();
}

// 무기 획득 시 통로 문 4개의 F 상호작용 해제.
// 플레이어가 이미 문 범위 안에 서 있다면 SetInteractionLocked(false) 내부에서
// 곧바로 "F - 진입" 프롬프트가 뜬다.
void AStartRoom::UnlockWeaponGateDoors()
{
    for (const TObjectPtr<ADoorBase>& Door : WeaponGateDoors)
    {
        if (IsValid(Door))
        {
            Door->SetInteractionLocked(false);
        }
    }
}

// 통로 문 진입이 끝났을 때 : 그 문을 사라지게 하고 StartRoom을 종료한다.
void AStartRoom::HandleWeaponGateDoorEntered(ADoorBase* Door)
{
    // 진입한 통로 문은 사라지게 한다 (일방 통행 — 통로로 나간 뒤엔 문이 필요 없음).
    if (IsValid(Door))
    {
        Door->OpenDoor();
    }

    // StartRoom이 아직 진행 중일 때만 종료시킨다.
    // (문이 4개라 다른 문을 또 통과하더라도 EndStartRoom이 중복 호출되지 않게 가드)
    if (!IsInProgress()) return;

    if (AGunFireGameMode* GFGameMode =
        GetWorld() ? GetWorld()->GetAuthGameMode<AGunFireGameMode>() : nullptr)
    {
        // StartRoom 종료 → CurrentRoom의 RoomState가 Cleared가 된다.
        // 이게 호출되지 않으면 CurrentRoom(StartRoom)이 InProgress로 남아
        // 다음 전투방의 TryPrepareRoom이 IsInProgress() 가드에서 막혀 몬스터가 스폰되지 않는다.
        GFGameMode->EndStartRoom();
    }
}

void AStartRoom::OnStart(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState)
{
    UE_LOG(LogTemp, Warning, TEXT("Start Room OnStart!!"));

    // 즉시 종료해야 한다면 시작하면서 EndRoom 호출
    if (bEndImmediately)
    {
        GFGameMode->EndStartRoom();
    }
}

void AStartRoom::OnEnd(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState)
{
    UE_LOG(LogTemp, Warning, TEXT("Start Room OnEnd!!"));
}
