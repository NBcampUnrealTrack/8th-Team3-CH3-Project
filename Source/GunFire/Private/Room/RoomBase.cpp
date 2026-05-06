#include "Room/RoomBase.h"

#include "GunFire/GunFireGameMode.h"
#include "Components/BoxComponent.h"

ARoomBase::ARoomBase()
{
 	PrimaryActorTick.bCanEverTick = false;

    Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
    SetRootComponent(Scene);

    EntryTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("EntryTrigger"));
    EntryTrigger->SetupAttachment(Scene);
    EntryTrigger->SetCollisionProfileName(TEXT("Trigger"));
    EntryTrigger->OnComponentBeginOverlap.AddDynamic(this, &ARoomBase::OnEntryTriggerBeginOverlap);

    RoomID = NAME_None;
    RoomType = ERoomType::Safe;
    NextRooms.Empty();
    bStarted = false;
    bCleared = false;
    bLocked = true;
}

void ARoomBase::StartRoom(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState)
{
    if (!GFGameMode || !GFGameState)
    {
        UE_LOG(LogTemp, Warning, TEXT("Game Mode or Game State Null"));
        return;
    }

    bStarted = true;
    OnStart(GFGameMode, GFGameState);
}

void ARoomBase::OnStart(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState)
{
}

void ARoomBase::EndRoom(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState)
{
    if (!GFGameMode || !GFGameState)
    {
        UE_LOG(LogTemp, Warning, TEXT("Game Mode or Game State Null"));
        return;
    }

    bCleared = true;
    OnEnd(GFGameMode, GFGameState);
}

void ARoomBase::OnEnd(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState)
{
}

// 다음 방이 이어져있는지 확인
bool ARoomBase::CanMoveNextRoom(const ARoomBase* NextRoom) const
{
    return NextRooms.Contains(NextRoom);
}

// 다음방이 있는지 확인
bool ARoomBase::HasNextRooms() const
{
    return !NextRooms.IsEmpty();
}

// 다음방을 진입 가능한 상태로 바꿈
void ARoomBase::UnlockNextRooms()
{
    for (ARoomBase* NextRoom : NextRooms)
    {
        if (NextRoom)
        {
            NextRoom->SetLocked(false);
        }
    }
}

ERoomType ARoomBase::GetRoomType() const
{
    return RoomType;
}

FName ARoomBase::GetRoomID() const
{
    return RoomID;
}

bool ARoomBase::IsStarted() const
{
    return bStarted;
}

bool ARoomBase::IsCleared() const
{
    return bCleared;
}

bool ARoomBase::IsLocked() const
{
    return bLocked;
}

void ARoomBase::SetLocked(bool bNewLocked)
{
    bLocked = bNewLocked;
}

void ARoomBase::OnEntryTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 플레이어와 충돌했다면
    if (OtherActor && OtherActor->ActorHasTag(TEXT("Player")))
    {
        if (AGunFireGameMode* GFGameMode = GetWorld() ?
                GetWorld()->GetAuthGameMode<AGunFireGameMode>() :
                nullptr)
        {
            // 게임모드에서 현재 방 진입 처리
            GFGameMode->TryEnterRoom(this);
        }
    }
}
