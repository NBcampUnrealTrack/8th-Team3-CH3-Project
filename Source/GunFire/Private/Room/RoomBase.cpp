#include "Room/RoomBase.h"

#include "GunFire/GunFireGameMode.h"
#include "Components/BoxComponent.h"
#include "Room/DoorBase.h"

ARoomBase::ARoomBase()
{
 	PrimaryActorTick.bCanEverTick = false;

    Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
    SetRootComponent(Scene);

    StartTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("EntryTrigger"));
    StartTrigger->SetupAttachment(Scene);
    StartTrigger->SetCollisionProfileName(TEXT("Trigger"));
    StartTrigger->SetBoxExtent(FVector(400.f, 400.f, 100.f));
    StartTrigger->OnComponentBeginOverlap.AddDynamic(this, &ARoomBase::OnEntryTriggerBeginOverlap);

    RoomID = NAME_None;
    RoomType = ERoomType::Safe;
    RoomState = ERoomState::Waiting;
}

void ARoomBase::StartRoom(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState)
{
    if (!GFGameMode || !GFGameState)
    {
        UE_LOG(LogTemp, Warning, TEXT("Game Mode or Game State Null"));
        return;
    }

    // 방과 연결된 문 닫기
    for (const auto& Door : EntranceDoors)
    {
        if (IsValid(Door))
        {
            Door->CloseDoor();
        }
    }

    RoomState = ERoomState::InProgress;
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

    // 방과 연결된 문 열기
    for (const auto& Door : EntranceDoors)
    {
        if (IsValid(Door))
        {
            Door->OpenDoor();
        }
    }

    RoomState = ERoomState::Cleared;
    OnEnd(GFGameMode, GFGameState);
}

void ARoomBase::OnEnd(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState)
{
}

FName ARoomBase::GetRoomID() const
{
    return RoomID;
}

ERoomType ARoomBase::GetRoomType() const
{
    return RoomType;
}

ERoomState ARoomBase::GetRoomState() const
{
    return RoomState;
}

bool ARoomBase::IsWaiting() const
{
    return RoomState == ERoomState::Waiting;
}

bool ARoomBase::IsPrepared() const
{
    return RoomState == ERoomState::Prepared;
}

bool ARoomBase::IsInProgress() const
{
    return RoomState == ERoomState::InProgress;
}

bool ARoomBase::IsCleared() const
{
    return RoomState == ERoomState::Cleared;
}

void ARoomBase::OnEntryTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 플레이어와 충돌했다면
    if (IsValid(OtherActor) && OtherActor->ActorHasTag(TEXT("Player")))
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
