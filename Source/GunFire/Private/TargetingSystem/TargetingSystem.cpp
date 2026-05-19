#include "TargetingSystem/TargetingSystem.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/EngineTypes.h"

UTargetingSystem::UTargetingSystem()
{
	PrimaryComponentTick.bCanEverTick = false;

}


void UTargetingSystem::BeginPlay()
{
	Super::BeginPlay();
}


void UTargetingSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTargetingSystem::FindAllTarget()
{
    AActor* OwnerActor = GetOwner();

    if (!OwnerActor) return;

    FVector Start = OwnerActor->GetActorLocation();
    FVector End = Start;

    // Object 타입 저장
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

    // 무시할 액터 배열
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(OwnerActor);

    // 히트 결과
    TArray<FHitResult> OutHit;

    // 트레이스 생성
    bool Hit = UKismetSystemLibrary::SphereTraceMultiForObjects(
        GetWorld(),
        Start,
        End,
        TraceDistance,
        ObjectTypes,
        false,
        ActorsToIgnore,
        EDrawDebugTrace::ForDuration,
        OutHit,
        true
    );

    if (Hit)
    {

    }
}

