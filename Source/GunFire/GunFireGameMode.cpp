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

void AGunFireGameMode::StartPlay()
{
    Super::StartPlay();

    if (APlayerController* PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr)
    {
        PlayerController->SetInputMode(FInputModeGameOnly());
        PlayerController->SetShowMouseCursor(false);
    }

    if (AGunFireGameState* GFGameState = GetGameState<AGunFireGameState>())
    {
        if (UGunFireGameInstance* GFGameInstance = GetGameInstance<UGunFireGameInstance>())
        {
            GFGameState->SetCurrentFloor(GFGameInstance->GetCurrentFloor());
            UE_LOG(LogTemp, Error, TEXT("플레이어 정보 입력 필요!!"));
        }
    }

    EnterInitialSafeRoom();
}

void AGunFireGameMode::EnterInitialSafeRoom()
{
    ASafeRoom* SafeRoom = FindInitialSafeRoom();
    if (!SafeRoom) return;
    if (SafeRoom->GetRoomType() != InitialRoomType) return;

    SafeRoom->SetLocked(false);
    CurrentRoom = SafeRoom;
    StartCurrentRoom();
}

void AGunFireGameMode::TryEnterRoom(ARoomBase* EnteredRoom)
{
    if (!EnteredRoom) return;
    if (!CanEnterRoom(EnteredRoom)) return;

    CurrentRoom = EnteredRoom;
    StartCurrentRoom();
}

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

    CurrentRoom->StartRoom(this, GFGameState);
}

void AGunFireGameMode::EndCurrentRoom()
{
    if (!IsValid(CurrentRoom)) return;

    AGunFireGameState* GFGameState = GetGameState<AGunFireGameState>();
    if (!GFGameState) return;

    GFGameState->SetCurrentRoomState(ERoomState::Cleared);
    CurrentRoom->EndRoom(this, GFGameState);

    UGunFireGameInstance* GFGameInstance = GetGameInstance<UGunFireGameInstance>();
    if (GFGameInstance)
    {
        FRoomData Data;
        Data.Floor = GFGameState->GetCurrentFloor();
        Data.RoomID = CurrentRoom->GetRoomID();
        Data.RoomType = CurrentRoom->GetRoomType();
        GFGameInstance->AddRoomData(Data);
    }

    if (CurrentRoom->GetRoomType() == ERoomType::Boss)
    {
        ClearGame();
        return;
    }

    CurrentRoom->UnlockNextRooms();

    if (!CurrentRoom->HasNextRooms())
    {
        UE_LOG(LogTemp, Warning, TEXT("Portal Activated!!"));
        ActivatePortal();
    }
}

void AGunFireGameMode::TryEnterNextFloor(FName NextLevelName)
{
    AGunFireGameState* GFGameState = GetGameState<AGunFireGameState>();
    if (!GFGameState) return;

    if (GFGameState->GetCurrentRoomState() != ERoomState::Cleared) return;
    if (!GFGameState->GetPortalActivated()) return;
    if (NextLevelName.IsNone()) return;

    UGunFireGameInstance* GFGameInstance = GetGameInstance<UGunFireGameInstance>();
    if (GFGameInstance)
    {
        GFGameInstance->MoveNextFloor();
        UE_LOG(LogTemp, Error, TEXT("플레이어 정보 인스턴스에 기록 필요"));
    }

    UGameplayStatics::OpenLevel(this, NextLevelName);
}

bool AGunFireGameMode::IsCurrentRoom(ARoomBase* Room) const
{
    return IsValid(Room) && CurrentRoom == Room;
}

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
    // AEnemyBase를 참조하는 CombatRoom의 함수 호출을 막아 빌드 에러를 방지합니다.
    /*
    if (ACombatRoom* CombatRoom = Cast<ACombatRoom>(CurrentRoom))
    {
        if (IsValid(CombatRoom))
        {
            CombatRoom->KillEnemyForTest();
        }
    }
    */
    UE_LOG(LogTemp, Warning, TEXT("KillEnemyForTest is disabled to fix AEnemyBase build error."));
}

void AGunFireGameMode::ActivatePortal()
{
    AGunFireGameState* GFGameState = GetGameState<AGunFireGameState>();
    if (!GFGameState) return;

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

bool AGunFireGameMode::CanEnterRoom(const ARoomBase* EnteredRoom)
{
    if (!EnteredRoom) return false;
    if (EnteredRoom->IsCleared()) return false;
    if (EnteredRoom->IsLocked()) return false;
    if (!CurrentRoom || !CurrentRoom->IsCleared()) return false;

    return CurrentRoom->CanMoveNextRoom(EnteredRoom);
}

ASafeRoom* AGunFireGameMode::FindInitialSafeRoom()
{
    if (InitialSafeRoomID.IsNone()) return nullptr;

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
