// Copyright Epic Games, Inc. All Rights Reserved.

#include "GunFireGameMode.h"

#include "EngineUtils.h"
#include "Game/GunFireGameInstance.h"
#include "Room/RoomBase.h"
#include "Game/GunFireGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Room/CombatRoom.h"
#include "Room/StartRoom.h"
#include "PlayerCharacter.h"
#include "GunFirePlayerController.h"
#include "LevelLayoutManager.h"
#include "StatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Item/InventoryComponent.h"
#include "Room/BossRoom.h"
#include "Sound/SoundSubsystem.h"
#include "Weapon/GunBase.h"
#include "Weapon/WeaponBase.h"

AGunFireGameMode::AGunFireGameMode()
	: Super()
{
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));

    DefaultPawnClass = APlayerCharacter::StaticClass();
    PlayerControllerClass = AGunFirePlayerController::StaticClass();
    GameStateClass = AGunFireGameState::StaticClass();

    ResultLevelName = TEXT("ResultLevel");
    CurrentRoom = nullptr;
    StartingRoom = nullptr;
    RequiredCombatRoomCount = 0;
    ClearedCombatRoomCount = 0;
    MaxRandomRelicRoomCount = 2;
    CurrentRandomRelicRoomCount = 0;

    CurrentGravityScale = 0.f;
    FloorBGMFadeInTime = 1.f;
}

// 레벨의 시작 처리
void AGunFireGameMode::StartPlay()
{
    Super::StartPlay();

    LockPlayer();
    ShowLoadingScreen();

    ALevelLayoutManager* LayoutManager = FindLevelLayoutManager();

    // 레벨 레이아웃 매니저 있으면 로드, 없으면 테스트할 수 있게 그냥 시작
    if (IsValid(LayoutManager))
    {
        LayoutManager->OnLayoutReady.AddDynamic(this, &AGunFireGameMode::HandleLayoutReady);
        LayoutManager->OnLoadingProgress.AddDynamic(this, &AGunFireGameMode::HandleLoadingProgress);
        LayoutManager->StartLevel();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("레벨 레이아웃 매니저를 배치해야합니다!!!"));
        HandleLayoutReady();
    }
}

// 레벨 시작 시 StartRoom에 진입
void AGunFireGameMode::EnterStartRoom()
{
    StartingRoom = FindStartRoom();
    if (!IsValid(StartingRoom)) return;

    APlayerController* PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
    if (!IsValid(PlayerController)) return;

    APawn* PlayerPawn = PlayerController->GetPawn();
    AActor* StartingPoint = StartingRoom->GetPlayerSpawnPoint();

    // StartRoom의 시작지점에 플레이어 배치
    if (IsValid(PlayerPawn))
    {
        if (IsValid(StartingPoint))
        {
            PlayerPawn->SetActorTransform(StartingPoint->GetActorTransform());
        }
        else
        {
            PlayerPawn->SetActorTransform(StartingRoom->GetActorTransform());
            UE_LOG(LogTemp, Warning, TEXT("시작지점 배치가 필요합니다"));
        }
    }

    CurrentRoom = StartingRoom;
    StartCurrentRoom();
}

// Start Room 을 종료하는 함수
// 이후에 상호작용이나 특정 조건에 의해 호출될 함수
void AGunFireGameMode::EndStartRoom()
{
    UE_LOG(LogTemp, Warning, TEXT("Start Room 종료!!"));
    EndCurrentRoom();
}

// 다음 방 준비 시도, 전투방일때만 사용
void AGunFireGameMode::TryPrepareRoom(ACombatRoom* EnteredRoom)
{
    if (!IsValid(EnteredRoom)) return;
    if (!EnteredRoom->IsWaiting()) return;

    // 현재 방이 진행중이지만 다른 방에 넘어가려할 경우 막음
    if (IsValid(CurrentRoom) && CurrentRoom->IsInProgress()) return;

    // 이미 다른 방을 준비 중이면 준비된 방이 아닌 방으로 진입하지 못하게 막음
    if (IsValid(CurrentRoom) && CurrentRoom->IsPrepared() && CurrentRoom != EnteredRoom) return;


    AGunFireGameState* GFGameState = GetGameState<AGunFireGameState>();
    if (!GFGameState) return;

    CurrentRoom = EnteredRoom;

    GFGameState->SetCurrentRoomType(CurrentRoom->GetRoomType());
    GFGameState->SetCurrentRoomState(ERoomState::Prepared);
    GFGameState->SetCurrentRoomID(CurrentRoom->GetRoomID());
    GFGameState->SetRemainingEnemyCount(0);

    EnteredRoom->PrepareRoom(this, GFGameState);
}

// 다음 방으로 진입 시도
void AGunFireGameMode::TryEnterRoom(ARoomBase* EnteredRoom)
{
    if (!IsValid(EnteredRoom)) return;
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
        GFGameInstance->AddClearedRoomCount(1);
    }

    // CombatRoom이면 클리어한 전투방 카운트 증가
    if (CurrentRoom->GetRoomType() == ERoomType::Combat
        || CurrentRoom->GetRoomType() == ERoomType::Boss)
    {
        ++ClearedCombatRoomCount;
        GFGameState->SetClearedCombatRoomCount(ClearedCombatRoomCount);

        if (GFGameInstance)
        {
            GFGameInstance->AddTotalClearedCombatRoomCount(1);
        }

        // 전투방 종료시 포탈  활성화 체크
        if (CurrentRoom->GetRoomType() == ERoomType::Combat)
        {
            // 요구하는 횟수를 만족하면 시작방에 포탈 활성화
            if (ClearedCombatRoomCount >= RequiredCombatRoomCount)
            {
                UE_LOG(LogTemp, Warning, TEXT("모든 전투방 클리어. 포탈을 활성화합니다!!"));
                ActivatePortal();
            }
        }
        else  // 보스방이 종료되었다면 결과창으로 이동하고 함수 종료
        {
            UE_LOG(LogTemp, Warning, TEXT("보스방 클리어"));
            if (ABossRoom* BossRoom = Cast<ABossRoom>(CurrentRoom))
            {
                BossRoom->ActivateResultPortal();
            }
        }
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
    if (!GFGameInstance) return;

    // 세션 정보 게임 인스턴스에 기록
    SaveSessionData();

    // 층 증가
    GFGameInstance->MoveNextFloor();

    // 포탈에서 지정한 다음 레벨로 이동
    UGameplayStatics::OpenLevel(this, NextLevelName);
}

bool AGunFireGameMode::IsCurrentRoom(ARoomBase* Room) const
{
    return IsValid(Room) && CurrentRoom == Room;
}

// 게임 클리어시 호출할 함수, 결과창으로 가는 함수
void AGunFireGameMode::ClearGame()
{
    if (ResultLevelName.IsNone())
    {
        UE_LOG(LogTemp, Error, TEXT("Result Level is None"));
        return;
    }

    UGameplayStatics::OpenLevel(this, ResultLevelName);
}

void AGunFireGameMode::GameOver()
{
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    AGunFirePlayerController* GFPlayerController = Cast<AGunFirePlayerController>(PlayerController);
    if (!IsValid(GFPlayerController)) return;

    GFPlayerController->ShowGameOverUI();
}

bool AGunFireGameMode::TryGenerateRandomRelicRoom()
{
    if (CurrentRandomRelicRoomCount >= MaxRandomRelicRoomCount)
    {
        return false;
    }

    ++CurrentRandomRelicRoomCount;
    return true;
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

    // Start Room 의 포탈 활성화
    if (IsValid(StartingRoom))
    {
        StartingRoom->ActivateFloorPortal();
    }
}

void AGunFireGameMode::HandleLayoutReady()
{
    // 전투 방 갯수 확인
    RequiredCombatRoomCount = CountCombatRooms();
    ClearedCombatRoomCount = 0;

    AGunFireGameState* GFGameState = GetGameState<AGunFireGameState>();
    if (!GFGameState) return;

    UGunFireGameInstance* GFGameInstance = GetGameInstance<UGunFireGameInstance>();
    if (!GFGameInstance) return;

    // 게임 스테이트 동기화
    GFGameState->StartFloor(GFGameInstance->GetCurrentFloor(), RequiredCombatRoomCount);

    UE_LOG(LogTemp, Warning, TEXT("%d 층"), GFGameInstance->GetCurrentFloor());
    UE_LOG(LogTemp, Warning, TEXT("필요한 전투방 클리어 횟수 : %d"), RequiredCombatRoomCount);

    // 기존 세션 정보 복구
    RestoreSessionData();

    // 시작방 진입
    EnterStartRoom();

    // 층별 BGM 재생
    PlayCurrentFloorBGM();

    // 플레이어 조작 가능하게 하고, 로딩 UI 가리기
    UnlockPlayer();
    HideLoadingScreen();
}

void AGunFireGameMode::HandleLoadingProgress(float Progress)
{
    APlayerController* PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
    if (!IsValid(PlayerController)) return;

    AGunFirePlayerController* GFPlayerController = Cast<AGunFirePlayerController>(PlayerController);
    if (IsValid(GFPlayerController))
    {
        GFPlayerController->UpdateLoadingProgress(Progress);
    }
}

// 다음 방이 진입할 수 있는지 확인하는 함수
bool AGunFireGameMode::CanEnterRoom(const ARoomBase* EnteredRoom)
{
    if (!IsValid(EnteredRoom)) return false;

    // 대기중이거나 준비된 방만 시작 가능
    // 진행중 or 클리어된 방은 시작 X
    if (!EnteredRoom->IsWaiting() && !EnteredRoom->IsPrepared()) return false;

    // 현재 방이 진행중이지만 다른 방에 넘어갔을 경우 시작 못하게 막음
    if (IsValid(CurrentRoom) && CurrentRoom->IsInProgress()) return false;

    // 이미 다른 방을 준비 중이면 준비된 방이 아닌 방으로 진입하지 못하게 막음
    if (IsValid(CurrentRoom) && CurrentRoom->IsPrepared() && CurrentRoom != EnteredRoom) return false;


    return true;
}

// StartRoom을 레벨에서 찾는 함수
AStartRoom* AGunFireGameMode::FindStartRoom()
{
    // TActorIterator 로 특정 클래스를 찾을 수 있음
    if (UWorld* World = GetWorld())
    {
        for (TActorIterator<AStartRoom> It(World); It; ++It)
        {
            AStartRoom* StartRoom = *It;
            if (IsValid(StartRoom))
            {
                return StartRoom;
            }
        }
    }
    return nullptr;
}

int32 AGunFireGameMode::CountCombatRooms()
{
    int32 Count = 0;

    // TActorIterator는 자식클래스까지 전부 찾으므로 RoomType으로 확인해야 함
    if (UWorld* World = GetWorld())
    {
        for (TActorIterator<ACombatRoom> It(World); It; ++It)
        {
            ACombatRoom* CombatRoom = *It;
            if (IsValid(CombatRoom) &&
                (CombatRoom->GetRoomType() == ERoomType::Combat ||
                CombatRoom->GetRoomType() == ERoomType::Boss))
            {
                ++Count;
            }
        }
    }
    return Count;
}

void AGunFireGameMode::SaveSessionData()
{
    UGunFireGameInstance* GFGameInstance = GetGameInstance<UGunFireGameInstance>();
    if (!GFGameInstance) return;

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    if (!IsValid(PlayerController)) return;

    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(PlayerController->GetPawn());
    if (!IsValid(PlayerCharacter)) return;

    if (UStatComponent* StatComponent = PlayerCharacter->FindComponentByClass<UStatComponent>())
    {
        FPlayerSessionData SessionData;
        SessionData.BaseStats = StatComponent->GetBaseStats();
        SessionData.CurrentHealth = StatComponent->GetCurrentHealth();

        GFGameInstance->SetPlayerSessionData(SessionData);
    }

    if (UWeaponComponent* WeaponComponent = PlayerCharacter->FindComponentByClass<UWeaponComponent>())
    {
        TArray<FEquippedWeaponSessionData> SessionData;

        for (int32 i = 0; i < static_cast<int32>(EWeaponSlot::Count); ++i)
        {
            EWeaponSlot Slot = static_cast<EWeaponSlot>(i);

            AWeaponBase* Weapon = WeaponComponent->GetWeapon(Slot);
            if (!IsValid(Weapon)) continue;

            FEquippedWeaponSessionData Data;
            Data.Slot = Slot;
            Data.WeaponClass = Weapon->GetClass();

            if (AGunBase* Gun = Cast<AGunBase>(Weapon))
            {
                Data.bHasGunData = true;
                Data.GunData.CurrentAmmo = Gun->GetCurrentAmmo();
                Data.GunData.RemainAmmo = Gun->GetRemainAmmo();
            }

            SessionData.Add(Data);
        }

        GFGameInstance->SetEquippedWeaponSessionData(SessionData);
    }

    if (UInventoryComponent* InventoryComponent = PlayerCharacter->FindComponentByClass<UInventoryComponent>())
    {
        FInventorySessionData SessionData;
        SessionData.OwnedPassives = InventoryComponent->OwnedPassives;
        SessionData.OwnedActives = InventoryComponent->OwnedActives;
        SessionData.OwnedMaterials = InventoryComponent->OwnedMaterials;

        GFGameInstance->SetInventorySessionData(SessionData);
    }
}

void AGunFireGameMode::RestoreSessionData()
{
    UGunFireGameInstance* GFGameInstance = GetGameInstance<UGunFireGameInstance>();
    if (!GFGameInstance) return;

    // 1층이면 복원할 필요 X
    if (GFGameInstance->GetCurrentFloor() <= 1) return;

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    if (!IsValid(PlayerController)) return;

    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(PlayerController->GetPawn());
    if (!IsValid(PlayerCharacter)) return;

    if (UStatComponent* StatComponent = PlayerCharacter->FindComponentByClass<UStatComponent>())
    {
        const FPlayerSessionData& SessionData = GFGameInstance->GetPlayerSessionData();

        StatComponent->SetBaseStats(SessionData.BaseStats);
        StatComponent->SetCurrentHealth(SessionData.CurrentHealth);
        StatComponent->CalculateFinalStats();
        StatComponent->RecoverStaminaMax();
    }

    if (UWeaponComponent* WeaponComponent = PlayerCharacter->FindComponentByClass<UWeaponComponent>())
    {
        const TArray<FEquippedWeaponSessionData>& SessionData = GFGameInstance->GetEquippedWeaponSessionData();

        for (const FEquippedWeaponSessionData& Data : SessionData)
        {
            if (!Data.WeaponClass) continue;

            AWeaponBase* NewWeapon = WeaponComponent->EquipWeapon(Data.WeaponClass, Data.Slot);
            if (!IsValid(NewWeapon)) continue;

            // 총일경우 총기 관련 데이터 복구
            if (Data.bHasGunData)
            {
                if (AGunBase* Gun = Cast<AGunBase>(NewWeapon))
                {
                    Gun->SetSessionData(Data.GunData);
                }
            }
        }
    }

    if (UInventoryComponent* InventoryComponent = PlayerCharacter->FindComponentByClass<UInventoryComponent>())
    {
        InventoryComponent->SetInventorySessionData(GFGameInstance->GetInventorySessionData());
    }
}

ALevelLayoutManager* AGunFireGameMode::FindLevelLayoutManager()
{
    if (UWorld* World = GetWorld())
    {
        for (TActorIterator<ALevelLayoutManager> It(World); It; ++It)
        {
            ALevelLayoutManager* LayoutManager = *It;
            if (IsValid(LayoutManager))
            {
                return LayoutManager;
            }
        }
    }
    return nullptr;
}

void AGunFireGameMode::ShowLoadingScreen()
{
    // 로딩창 가리기
    APlayerController* PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
    if (!IsValid(PlayerController)) return;

    AGunFirePlayerController* GFPlayerController = Cast<AGunFirePlayerController>(PlayerController);
    if (IsValid(GFPlayerController))
    {
        GFPlayerController->ShowLoadingScreen();
    }
}

void AGunFireGameMode::HideLoadingScreen()
{
    // 로딩창 가리기
    APlayerController* PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
    if (!IsValid(PlayerController)) return;

    AGunFirePlayerController* GFPlayerController = Cast<AGunFirePlayerController>(PlayerController);
    if (IsValid(GFPlayerController))
    {
        GFPlayerController->HideLoadingScreen();
    }
}

void AGunFireGameMode::LockPlayer()
{
    APlayerController* PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
    if (!IsValid(PlayerController)) return;

    ACharacter* Character = Cast<APlayerCharacter>(PlayerController->GetPawn());
    if (!IsValid(Character)) return;

    UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement();
    if (!IsValid(MovementComponent)) return;

    // 이동, 마우스 회전 금지
    PlayerController->SetIgnoreLookInput(true);
    PlayerController->SetIgnoreMoveInput(true);

    // 움직임을 멈추고 이동 불가능하게 만들기, 중력 0으로
    MovementComponent->StopMovementImmediately();
    MovementComponent->DisableMovement();
    CurrentGravityScale = MovementComponent->GravityScale;
    MovementComponent->GravityScale = 0.f;
}

void AGunFireGameMode::UnlockPlayer()
{
    APlayerController* PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
    if (!IsValid(PlayerController)) return;

    ACharacter* Character = Cast<APlayerCharacter>(PlayerController->GetPawn());
    if (!IsValid(Character)) return;

    UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement();
    if (!IsValid(MovementComponent)) return;

    // 움직임과 중력 되돌리기
    MovementComponent->SetMovementMode(MOVE_Walking);
    MovementComponent->GravityScale = CurrentGravityScale;

    // 이동, 마우스 회전 되돌리기
    PlayerController->SetIgnoreLookInput(false);
    PlayerController->SetIgnoreMoveInput(false);
}

void AGunFireGameMode::PlayCurrentFloorBGM()
{
    UGunFireGameInstance* GFGameInstance = GetGameInstance<UGunFireGameInstance>();
    if (!GFGameInstance) return;

    int32 CurrentFloor = GFGameInstance->GetCurrentFloor();
    int32 BGMIndex = CurrentFloor - 1;

    if (!FloorBGMs.IsValidIndex(BGMIndex)) return;

    USoundBase* BGM = FloorBGMs[BGMIndex];
    if (!BGM) return;

    USoundSubsystem* SoundSubsystem = GetWorld() ? GetWorld()->GetSubsystem<USoundSubsystem>() : nullptr;
    if (!SoundSubsystem) return;

    SoundSubsystem->PlayBGM(BGM, FloorBGMFadeInTime);
}
