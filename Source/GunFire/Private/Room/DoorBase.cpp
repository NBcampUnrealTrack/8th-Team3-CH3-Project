#include "Room/DoorBase.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "Components/WidgetComponent.h"

ADoorBase::ADoorBase()
{
    // Tick은 진입 보간 중에만 사용 — 평소엔 꺼 둔다 (성능).
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;

    Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
    SetRootComponent(Scene);

    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    StaticMesh->SetupAttachment(Scene);
    // 콜리전은 그대로 Block — 진입 전엔 못 들어가고, 진입 후엔 못 나가는 게 자동으로 된다.
    StaticMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));

    // 플레이어 감지용 트리거 박스 (에디터/BP에서 크기·위치 조정)
    TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
    TriggerVolume->SetupAttachment(Scene);
    TriggerVolume->SetBoxExtent(FVector(150.f, 120.f, 120.f));
    TriggerVolume->SetCollisionProfileName(TEXT("Trigger"));
    TriggerVolume->SetGenerateOverlapEvents(true);

    // 진입 방향 화살표 — 기본값은 액터 정면. 에디터에서 회전시켜 통로 안쪽을 가리키게 한다.
    EntryDirectionArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("EntryDirectionArrow"));
    EntryDirectionArrow->SetupAttachment(Scene);
    EntryDirectionArrow->SetArrowColor(FLinearColor::Yellow);

    // 위젯 컴포넌트
    InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
    InteractionWidget->SetupAttachment(RootComponent);

    InteractionWidget->SetVisibility(false);
    InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
}

void ADoorBase::BeginPlay()
{
    Super::BeginPlay();

    // 초기 상태는 생성자 기본값 그대로 — 닫힘 + 상호작용 가능.
    // (여기서 CloseDoor를 부르면 처음부터 잠겨서 F 진입이 막힌다)
    // StartRoom 통로 문처럼 "처음엔 잠금"이 필요하면 소유 액터가 BeginPlay에서
    // SetInteractionLocked(true)를 호출한다 — bLocked는 트리거/콜리전을 건드리지 않으므로 안전.

    TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ADoorBase::OnTriggerBeginOverlap);
    TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &ADoorBase::OnTriggerEndOverlap);
}

void ADoorBase::OnTriggerBeginOverlap(UPrimitiveComponent* /*OverlappedComp*/, AActor* OtherActor,
    UPrimitiveComponent* /*OtherComp*/, int32 /*OtherBodyIndex*/, bool /*bFromSweep*/,
    const FHitResult& /*SweepResult*/)
{
    if (bHasEntered) return;   // 이미 들어왔으면 무시

    APawn* Pawn = Cast<APawn>(OtherActor);
    if (!Pawn || !Pawn->IsPlayerControlled()) return;

    // 잠겨 있어도 "어떤 폰이 범위 안에 있는지"는 기억해 둔다.
    // 무기 획득 등으로 SetInteractionLocked(false)가 호출되면 그 시점에 프롬프트를 띄운다.
    OverlappingPawn = Pawn;

    if (!bLocked)
    {
        EnableInteractionForCurrentPawn();   // BP: "F - 진입" UI 표시 + F 바인딩
    }
}

void ADoorBase::OnTriggerEndOverlap(UPrimitiveComponent* /*OverlappedComp*/, AActor* OtherActor,
    UPrimitiveComponent* /*OtherComp*/, int32 /*OtherBodyIndex*/)
{
    // 진입 보간 중에는 트리거를 벗어나도 무시 (보간이 끊기지 않게)
    if (bIsEntering) return;
    if (OtherActor != OverlappingPawn) return;

    if (APawn* Pawn = OverlappingPawn)
    {
        if (APlayerController* PC = Cast<APlayerController>(Pawn->GetController()))
        {
            DisableInput(PC);
        }
    }

    OverlappingPawn = nullptr;
    OnPlayerInRange(false);  // BP: UI 숨김
}

void ADoorBase::EnableInteractionForCurrentPawn()
{
    APawn* Pawn = OverlappingPawn;
    if (!Pawn) return;

    APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
    if (!PC) return;

    // F키를 이 액터에 직접 바인딩 — 프로젝트 입력 설정을 안 건드려도 동작한다.
    EnableInput(PC);
    if (!bInputBound && InputComponent)
    {
        InputComponent->BindKey(EKeys::F, IE_Pressed, this, &ADoorBase::TryEnter);
        bInputBound = true;
    }

    OnPlayerInRange(true);   // BP: "F - 진입" UI 표시
}

void ADoorBase::SetInteractionLocked(bool bNewLocked)
{
    if (bLocked == bNewLocked) return;
    bLocked = bNewLocked;

    if (bLocked)
    {
        // 잠금 : "F - 진입" 프롬프트를 숨기고 F 입력을 막는다.
        // 트리거 오버랩 자체와 OverlappingPawn은 그대로 둔다 — 나중에 해제될 때
        // 플레이어가 이미 범위 안에 서 있으면 곧바로 프롬프트를 띄우기 위해서다.
        if (APawn* Pawn = OverlappingPawn)
        {
            if (APlayerController* PC = Cast<APlayerController>(Pawn->GetController()))
            {
                DisableInput(PC);
            }
        }
        OnPlayerInRange(false);
    }
    else
    {
        // 해제 : 이미 트리거 안에 플레이어가 서 있다면 즉시 프롬프트를 띄운다.
        // (잠긴 동안 OnTriggerBeginOverlap이 OverlappingPawn만 기록해 둔 상태이므로,
        //  다시 오버랩 이벤트가 들어오길 기다리지 않고 여기서 활성화한다.)
        if (!bHasEntered && OverlappingPawn)
        {
            EnableInteractionForCurrentPawn();
        }
    }
}

void ADoorBase::TryEnter()
{
    // 잠긴 문은 F 진입 불가
    if (bLocked) return;

    // 범위 안 + 아직 진입 안 함 + 진행 중 아님 일 때만
    if (bHasEntered || bIsEntering || !OverlappingPawn) return;

    APawn* Pawn = OverlappingPawn;

    bIsEntering = true;
    bHasEntered = true;
    OnPlayerInRange(false);

    // 진입 중 플레이어 입력 잠금
    if (APlayerController* PC = Cast<APlayerController>(Pawn->GetController()))
    {
        Pawn->DisableInput(PC);
    }

    // 캐릭터 무브먼트가 위치 보간과 충돌하지 않게 잠시 정지
    if (ACharacter* Character = Cast<ACharacter>(Pawn))
    {
        if (UCharacterMovementComponent* Move = Character->GetCharacterMovement())
        {
            Move->StopMovementImmediately();
            Move->DisableMovement();
        }
    }

    // 시작/도착 위치 계산. EntryDirectionArrow가 가리키는 방향으로 EntryDistance만큼.
    EntryStartLocation = Pawn->GetActorLocation();
    EntryTargetLocation = EntryStartLocation + EntryDirectionArrow->GetForwardVector() * EntryDistance;

    EntryElapsed = 0.f;
    SetActorTickEnabled(true);   // 보간 시작
    OnEntryStarted();            // BP: 걷기 몽타주 재생 등
}

void ADoorBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsEntering || !OverlappingPawn) return;

    EntryElapsed += DeltaTime;
    const float Alpha = FMath::Clamp(EntryElapsed / FMath::Max(EntryDuration, 0.01f), 0.f, 1.f);
    // 가속/감속이 들어간 부드러운 진입
    const float Smooth = FMath::InterpEaseInOut(0.f, 1.f, Alpha, 2.f);

    const FVector NewLocation = FMath::Lerp(EntryStartLocation, EntryTargetLocation, Smooth);
    // sweep=false: 문 콜리전을 통과해서 이동시킨다.
    OverlappingPawn->SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);

    if (Alpha >= 1.f)
    {
        FinishEntry();
    }
}

void ADoorBase::FinishEntry()
{
    bIsEntering = false;
    SetActorTickEnabled(false);

    if (APawn* Pawn = OverlappingPawn)
    {
        // 무브먼트 복구
        if (ACharacter* Character = Cast<ACharacter>(Pawn))
        {
            if (UCharacterMovementComponent* Move = Character->GetCharacterMovement())
            {
                Move->SetMovementMode(MOVE_Walking);
            }
        }
        // 입력 복구
        if (APlayerController* PC = Cast<APlayerController>(Pawn->GetController()))
        {
            Pawn->EnableInput(PC);
            DisableInput(PC);   // 이 문은 더 이상 F 입력을 받을 필요 없음
        }
    }

    OverlappingPawn = nullptr;
    OnEntryFinished();   // BP: 몽타주 정지 등

    // 진입 보간 완료를 알린다.
    // StartRoom은 이 시점에 바인딩된 콜백에서 EndStartRoom() + 통로 문 정리를 수행한다.
    // 이 시점엔 bIsEntering=false, OverlappingPawn=nullptr 이므로
    // 콜백 안에서 OpenDoor()를 호출해도 진입 보간과 충돌하지 않는다.
    OnDoorEntered.Broadcast(this);
}

// BlueprintNativeEvent : 본체(디스패처) ADoorBase::OpenDoor()는 UHT가 생성한다.
// C++ 구현부는 반드시 _Implementation 접미사로 정의해야 한다.
void ADoorBase::OpenDoor_Implementation()
{
    UE_LOG(LogTemp, Warning, TEXT("[DoorBase] OpenDoor 실행 - %s"), *GetName());

    // 문 열림 — 통과 가능 + 비표시. 적을 다 잡으면 Room 쪽에서 호출된다.
    bHasEntered = true;   // 열린 뒤엔 F 진입 시퀀스가 다시 돌지 않게

    StaticMesh->SetVisibility(false);
    StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 열린 문에 "F - 진입" 프롬프트가 뜨지 않게 트리거를 끈다.
    TriggerVolume->SetGenerateOverlapEvents(false);

    // 진입 보간 중이 아닐 때만 입력/프롬프트를 정리한다.
    // 진입 도중에 문이 열리면(예: 적 없는 랜덤방은 진입 즉시 방이 끝남)
    // 여기서 OverlappingPawn을 비우면 Tick의 보간이 끊겨 플레이어가
    // 영구히 멈춘다. 진입 중이면 그대로 두고, 보간이 끝날 때
    // FinishEntry가 입력·이동을 복구한다.
    if (!bIsEntering)
    {
        if (APawn* Pawn = OverlappingPawn)
        {
            if (APlayerController* PC = Cast<APlayerController>(Pawn->GetController()))
            {
                DisableInput(PC);
            }
        }
        OverlappingPawn = nullptr;
        OnPlayerInRange(false);
    }

}

// BlueprintNativeEvent : 본체(디스패처) ADoorBase::CloseDoor()는 UHT가 생성한다.
// C++ 구현부는 반드시 _Implementation 접미사로 정의해야 한다.
void ADoorBase::CloseDoor_Implementation()
{
    UE_LOG(LogTemp, Warning, TEXT("[DoorBase] CloseDoor 실행 - %s"), *GetName());

    // 잠금 — 보이고 막힌 상태는 유지하되, F 상호작용을 차단한다.
    // 방(Room)이 시작될 때 호출되어 그 방의 문들을 전부 잠근다.
    StaticMesh->SetVisibility(true);
    StaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    // 새 오버랩(= F 프롬프트) 차단
    TriggerVolume->SetGenerateOverlapEvents(false);

    // 진입 보간 중이면 건드리지 않는다 (보간이 끊기지 않게).
    // 진입 중인 문은 TryEnter에서 이미 bHasEntered = true 상태다.
    if (!bIsEntering)
    {
        if (APawn* Pawn = OverlappingPawn)
        {
            if (APlayerController* PC = Cast<APlayerController>(Pawn->GetController()))
            {
                DisableInput(PC);
            }
        }
        OverlappingPawn = nullptr;
        OnPlayerInRange(false);
        bHasEntered = true;   // 이후 F 진입 시퀀스가 돌지 않게 잠금
    }

}
