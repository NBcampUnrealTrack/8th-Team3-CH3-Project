#include "Interactables/Portal.h"

#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "GunFire/GunFireGameMode.h"

APortal::APortal()
{
	PrimaryActorTick.bCanEverTick = false;

    Scene = CreateDefaultSubobject<USceneComponent>("Scene");
    SetRootComponent(Scene);

    Collision = CreateDefaultSubobject<UBoxComponent>("Collision");
    Collision->SetupAttachment(Scene);
    Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    Collision->OnComponentBeginOverlap.AddDynamic(this, &APortal::OnPortalBeginOverlap);

    PortalEffect = CreateDefaultSubobject<UNiagaraComponent>("PortalEffect");
    PortalEffect->SetupAttachment(Scene);

    TargetLevelName = NAME_None;
    bActive = false;
    PortalType = EPortalType::NextFloor;
}

void APortal::BeginPlay()
{
    Super::BeginPlay();

    // 시작시 포탈 끄고 시작
    SetActive(false);
}

void APortal::SetActive(bool NewValue)
{
    bActive = NewValue;

    // 충돌 여부 변경, 나이아가라 활성화/비활성화
    if (bActive)
    {
        Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        PortalEffect->Activate();
    }
    else
    {
        Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        PortalEffect->Deactivate();
    }

    // 가시성 변경, 자식 컴포넌트들까지 가시성 변경함
    PortalEffect->SetVisibility(bActive, true);
}

void APortal::OnPortalBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                   int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!bActive) return;
    if (TargetLevelName.IsNone()) return;

    // 플레이어와 충돌인 경우에만
    if (IsValid(OtherActor) && OtherActor->ActorHasTag(TEXT("Player")))
    {
        if (AGunFireGameMode* GFGameMode = GetWorld() ?
                GetWorld()->GetAuthGameMode<AGunFireGameMode>() :
                nullptr)
        {
            if (PortalType == EPortalType::NextFloor)
            {
                GFGameMode->TryEnterNextFloor(TargetLevelName);
            }
            else if (PortalType == EPortalType::ClearGame)
            {
                GFGameMode->ClearGame();
            }
        }
    }
}

