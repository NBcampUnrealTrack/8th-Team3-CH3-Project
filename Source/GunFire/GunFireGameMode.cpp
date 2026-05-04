// Copyright Epic Games, Inc. All Rights Reserved.

#include "GunFireGameMode.h"
#include "GunFireCharacter.h"
#include "Room/RoomBase.h"
#include "GunFireGameState.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AGunFireGameMode::AGunFireGameMode()
	: Super()
{

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));

    DefaultPawnClass = PlayerPawnClassFinder.Class;
    GameStateClass = AGunFireGameState::StaticClass();
}

void AGunFireGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);

    // 랜덤으로 받아와야 함
    InitialRoomType = ERoomType::Combat;
    UE_LOG(LogTemp, Warning, TEXT("Game Mode Initialize"));
}

void AGunFireGameMode::StartPlay()
{
    Super::StartPlay();

    // OpenLevel 필요
    StartRoom(InitialRoomType);
}

void AGunFireGameMode::StartRoom(ERoomType RoomType)
{
    AGunFireGameState* GFGameState = GetGameState<AGunFireGameState>();
    if (!GFGameState) return;

    GFGameState->SetRoomType(RoomType);
    GFGameState->SetRoomState(ERoomState::InProgress);
    GFGameState->SetRemainingEnemyCount(0);
    GFGameState->SetPortalActivated(false);

    // 현재 방을 시작
    if (ARoomBase* CurrentRoom = FindRoom())
    {
        CurrentRoom->StartRoom(this, GFGameState);
    }
}

void AGunFireGameMode::EndRoom()
{
    AGunFireGameState* GFGameState = GetGameState<AGunFireGameState>();
    if (!GFGameState) return;

    GFGameState->SetRoomState(ERoomState::Cleared);

    // 현재 방을 종료함
    if (ARoomBase* CurrentRoom = FindRoom())
    {
        CurrentRoom->EndRoom(this, GFGameState);
    }

    ActivatePortal();
}

void AGunFireGameMode::ActivatePortal()
{
    AGunFireGameState* GFGameState = GetGameState<AGunFireGameState>();
    if (!GFGameState) return;

    GFGameState->SetPortalActivated(true);
}

ARoomBase* AGunFireGameMode::FindRoom() const
{
    // 레벨에 배치된 RoomBase 클래스 타입의 액터를 찾아옴
    AActor* FoundActor = UGameplayStatics::GetActorOfClass(this, ARoomBase::StaticClass());
    if (!FoundActor) return nullptr;

    // 캐스팅해서 RoomBase 반환
    return Cast<ARoomBase>(FoundActor);
}





