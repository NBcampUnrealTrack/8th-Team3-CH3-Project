#include "Room/SafeRoom.h"

#include "Components/BoxComponent.h"
#include "GunFire/GunFireGameMode.h"

ASafeRoom::ASafeRoom()
{
    ExitTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("ExitTrigger"));
    ExitTrigger->SetupAttachment(Scene);
    ExitTrigger->SetCollisionProfileName(TEXT("Trigger"));
    ExitTrigger->OnComponentBeginOverlap.AddDynamic(this, &ASafeRoom::OnExitTriggerBeginOverlap);

    RoomType = ERoomType::Safe;
}

void ASafeRoom::OnStart(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState)
{
    UE_LOG(LogTemp, Warning, TEXT("Safe Room Start"));
}

void ASafeRoom::OnEnd(AGunFireGameMode* GFGameMode, AGunFireGameState* GFGameState)
{
    UE_LOG(LogTemp, Warning, TEXT("Safe Room End"));
}

// 휴식방의 경우 별도의 탈출 트리거 박스를 둬서 나가는 것 판정 처리
void ASafeRoom::OnExitTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor->ActorHasTag("Player"))
    {
        if (AGunFireGameMode* GFGameMode = GetWorld() ?
        GetWorld()->GetAuthGameMode<AGunFireGameMode>() :
        nullptr)
        {
            // SafeRoom의 경우 트리거 Overlap 이 해당 방 빠져나오도록 함
            GFGameMode->EndCurrentRoom();
        }
    }
}

