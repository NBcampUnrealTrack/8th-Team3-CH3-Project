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

    PortalDashEffect = CreateDefaultSubobject<UNiagaraComponent>("PortalDashEffect");
    PortalDashEffect->SetupAttachment(PortalEffect);
}

void APortal::OnPortalBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 플레이어와 충돌인 경우에만
    if (OtherActor && OtherActor->ActorHasTag(TEXT("Player")))
    {
        if (AGunFireGameMode* GFGameMode = GetWorld() ?
                GetWorld()->GetAuthGameMode<AGunFireGameMode>() :
                nullptr)
        {
            UE_LOG(LogTemp, Warning, TEXT("Portal Overlapped"));
            GFGameMode->TryEnterNextFloor();
        }
    }
}

