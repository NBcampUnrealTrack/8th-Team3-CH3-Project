// Copyright Epic Games, Inc. All Rights Reserved.

#include "GunFireGameMode.h"

#include "EngineUtils.h"
#include "Game/GunFireGameInstance.h"
#include "Room/RoomBase.h"
#include "Game/GunFireGameState.h"
#include "Interactables/Portal.h"
#include "Kismet/GameplayStatics.h"
#include "Room/CombatRoom.h"
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
    ResultLevelName = TEXT("ResultLevel");
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

    if (APlayerController* PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr)
    {
        PlayerController->SetInputMode(FInputModeGameOnly());
        PlayerController->SetShowMouseCursor(false);
    }

    // 게임 인스턴스 층 동기화
    if (AGunFireGameState* GFGameState = GetGameState<AGunFireGameState>())
    {
        if (UGunFireGameInstance* GFGameInstance = GetGameInstance<UGunFireGameInstance>())
        {
            GFGameState->SetCurrentFloor(GFGameInstance->GetCurrentFloor());

            // 플레이어 정보 동기화 필요
            UE_LOG(LogTemp, Error, TEXT("플레이어 정보 입력 필요!!"));
        }
    }


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


    // 게임 인스턴스에 클리어한 방 정보 저장
    UGunFireGameInstance* GFGameInstance = GetGameInstance<UGunFireGameInstance>();
    if (GFGameInstance)
    {
        FRoomData Data;
        Data.Floor = GFGameState->GetCurrentFloor();
        Data.RoomID = CurrentRoom->GetRoomID();
        Data.RoomType = CurrentRoom->GetRoomType();

        GFGameInstance->AddRoomData(Data);
    }

    // 보스방이 종료되었다면 결과창으로 이동하고 함수 종료
    if (CurrentRoom->GetRoomType() == ERoomType::Boss)
    {
        ClearGame();
        return;
    }

    // 다음 방들 진입 가능한 상태로 전환
    CurrentRoom->UnlockNextRooms();

    // 다음방이 없다면 마지막방이므로 포탈 활성화
    if (!CurrentRoom->HasNextRooms())
    {
        UE_LOG(LogTemp, Warning, TEXT("Portal Activated!!"));
        ActivatePortal();
    }
}

// 다음층으로 진입 시도하는 함수
void AGunFireGameMode::TryEnterNextFloor(FName NextLevelName)
{
    AGunFireGameState* GFGameState = GetGameState<AGunFireGameState>();
    if (!GFGameState) return;

    // 클리어 된 상태가 아니라면 다음 방 진입 X
    if (GFGameState->GetCurrentRoomState() != ERoomState::Cleared) return;

    // 포탈이 활성화 된 상태가 아니라면 진입 X
    if (!GFGameState->GetPortalActivated()) return;

    // 다음 레벨 이름이 지정되어있지 않으면 진입 X
    if (NextLevelName.IsNone()) return;


    // 게임 인스턴스 층 변경
    UGunFireGameInstance* GFGameInstance = GetGameInstance<UGunFireGameInstance>();
    if (GFGameInstance)
    {
        GFGameInstance->MoveNextFloor();

        // 플레이어 정보 동기화 필요
        UE_LOG(LogTemp, Error, TEXT("플레이어 정보 인스턴스에 기록 필요"));
    }


    // 포탈에서 지정한 다음 레벨로 이동
    UGameplayStatics::OpenLevel(this, NextLevelName);

    // 층 이동 Instance 동기화 필요함
}

bool AGunFireGameMode::IsCurrentRoom(ARoomBase* Room) const
{
    return IsValid(Room) && CurrentRoom == Room;
}

// 게임 클리어시 호출할 함수, 결과창으로 가는 함수
void AGunFireGameMode::ClearGame()
{
    GoToResultLevel(ESessionResult::Victory);
}

void AGunFireGameMode::GameOver()
{
    GoToResultLevel(ESessionResult::Death);
}

void AGunFireGameMode::KillEnemyForTest()
{
    if (ACombatRoom* CombatRoom = Cast<ACombatRoom>(CurrentRoom))
    {
        if (IsValid(CombatRoom))
        {
            CombatRoom->KillEnemyForTest();
        }
    }
}

// 포탈 활성화 함수
void AGunFireGameMode::ActivatePortal()
{
    AGunFireGameState* GFGameState = GetGameState<AGunFireGameState>();
    if (!GFGameState) return;

    // GameState 동기화
    GFGameState->SetPortalActivated(true);

    UWorld* World = GetWorld();
    if (!World) return;

    for (TActorIterator<APortal> It(World); It; ++It)
    {
        APortal* Portal = *It;
        if (IsValid(Portal))
        {
            Portal->SetActive(true);
        }
    }
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
            if (IsValid(SafeRoom) && SafeRoom->GetRoomID() == InitialSafeRoomID)
            {
                return SafeRoom;
            }
        }
    }

    return nullptr;
}

void AGunFireGameMode::GoToResultLevel(ESessionResult Result)
{
    // 게임 인스턴스에서 세션 끝내기
    if (UGunFireGameInstance* GFGameInstance = GetGameInstance<UGunFireGameInstance>())
    {
        GFGameInstance->FinishSession(Result);
    }

    if (ResultLevelName.IsNone())
    {
        UE_LOG(LogTemp, Error, TEXT("Result Level is None"));
        return;
    }

    UGameplayStatics::OpenLevel(this, ResultLevelName);
}
