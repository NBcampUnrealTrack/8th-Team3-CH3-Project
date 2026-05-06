// Copyright Epic Games, Inc. All Rights Reserved.

#include "GunFireGameMode.h"

#include "EngineUtils.h"
#include "Room/RoomBase.h"
#include "GunFireGameState.h"
#include "Room/SafeRoom.h"
#include "UObject/ConstructorHelpers.h"

AGunFireGameMode::AGunFireGameMode()
	: Super()
{

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));

    DefaultPawnClass = PlayerPawnClassFinder.Class;
    GameStateClass = AGunFireGameState::StaticClass();
    InitialRoomType = ERoomType::Safe;
    InitialSafeRoomID = TEXT("StartSafeRoom");
    CurrentRoom = nullptr;
}

void AGunFireGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);

    UE_LOG(LogTemp, Warning, TEXT("Game Mode Initialize"));
}

// 레벨의 시작 처리
void AGunFireGameMode::StartPlay()
{
    Super::StartPlay();

    EnterInitialSafeRoom();
}

// 레벨 시작 시 초기 방에 진입, 현재 레벨마다 초기 방은 휴식방
void AGunFireGameMode::EnterInitialSafeRoom()
{
    ASafeRoom* SafeRoom = FindInitialSafeRoom();
    if (!SafeRoom) return;

    if (SafeRoom->GetRoomType() != InitialRoomType) return;

    SafeRoom->SetLocked(false);

    CurrentRoom = SafeRoom;
    StartCurrentRoom();
}

// 다음 방으로 진입 시도
void AGunFireGameMode::TryEnterRoom(ARoomBase* EnteredRoom)
{
    if (!EnteredRoom) return;
    if (!CanEnterRoom(EnteredRoom)) return;

    CurrentRoom = EnteredRoom;
    StartCurrentRoom();
}

// 현재 방의 시작을 처리하는 함수
void AGunFireGameMode::StartCurrentRoom()
{
    if (!IsValid(CurrentRoom)) return;

    AGunFireGameState* GFGameState = GetGameState<AGunFireGameState>();
    if (!GFGameState) return;

    GFGameState->SetCurrentRoomType(CurrentRoom->GetRoomType());
    GFGameState->SetCurrentRoomState(ERoomState::InProgress);
    GFGameState->SetCurrentRoomID(CurrentRoom->GetRoomID());
    GFGameState->SetRemainingEnemyCount(0);
    GFGameState->SetPortalActivated(false);

    // 현재 방을 시작처리함
    CurrentRoom->StartRoom(this, GFGameState);
}

// 현채 방을 정리하는 함수
void AGunFireGameMode::EndCurrentRoom()
{
    if (!IsValid(CurrentRoom)) return;

    AGunFireGameState* GFGameState = GetGameState<AGunFireGameState>();
    if (!GFGameState) return;

    GFGameState->SetCurrentRoomState(ERoomState::Cleared);

    // 현재 방을 종료함
    CurrentRoom->EndRoom(this, GFGameState);

    // 다음 방들 진입 가능한 상태로 전환
    CurrentRoom->UnlockNextRooms();

    // 다음방이 없다면 마지막방이므로 포탈 활성화
    if (!CurrentRoom->HasNextRooms())
    {
        ActivatePortal();
    }
}

// 다음층으로 진입 시도하는 함수
void AGunFireGameMode::TryEnterNextFloor()
{
    AGunFireGameState* GFGameState = GetGameState<AGunFireGameState>();
    if (!GFGameState) return;

    // 클리어 된 상태가 아니라면 다음 방 진입 X
    if (GFGameState->GetCurrentRoomState() != ERoomState::Cleared) return;

    // 포탈이 활성화 된 상태가 아니라면 진입 X
    if (!GFGameState->GetPortalActivated()) return;

    UE_LOG(LogTemp, Warning, TEXT("EnterNextRoom"));

    // 층 증가 및 다음 층에 해당하는 레벨 OpenLevel
}

// 포탈 활성화
void AGunFireGameMode::ActivatePortal()
{
    AGunFireGameState* GFGameState = GetGameState<AGunFireGameState>();
    if (!GFGameState) return;

    GFGameState->SetPortalActivated(true);
}

// 다음 방이 진입할 수 있는지 확인하는 함수
bool AGunFireGameMode::CanEnterRoom(const ARoomBase* EnteredRoom)
{
    // 진입하려는 방 확인
    if (!EnteredRoom) return false;
    if (EnteredRoom->IsCleared()) return false;
    if (EnteredRoom->IsLocked()) return false;

    // 현재 방 상태 확인
    if (!CurrentRoom || !CurrentRoom->IsCleared()) return false;

    // 다음 방으로 이어져있는지 확인
    return CurrentRoom->CanMoveNextRoom(EnteredRoom);
}

// 맨 처음 휴식방 찾는 함수
ASafeRoom* AGunFireGameMode::FindInitialSafeRoom()
{
    // 초기 SafeRoomID 를 초기화하지 않았다면 nullptr
    if (InitialSafeRoomID.IsNone()) return nullptr;

    // TActorIterator 로 특정 클래스를 찾을 수 있음
    if (UWorld* World = GetWorld())
    {
        for (TActorIterator<ASafeRoom> It(World); It; ++It)
        {
            ASafeRoom* SafeRoom = *It;
            if (SafeRoom && SafeRoom->GetRoomID() == InitialSafeRoomID)
            {
                return SafeRoom;
            }
        }
    }

    return nullptr;
}
