#include "Room/DoorBase.h"

ADoorBase::ADoorBase()
{
    Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
    SetRootComponent(Scene);

    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    StaticMesh->SetupAttachment(Scene);
    StaticMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
}

void ADoorBase::BeginPlay()
{
    Super::BeginPlay();

    OpenDoor();
}

void ADoorBase::OpenDoor()
{
    OnOpenDoor();
}

void ADoorBase::CloseDoor()
{
    OnCloseDoor();
}

void ADoorBase::OnCloseDoor_Implementation()
{
    // C++ 동작이 필요하면 작성
}

void ADoorBase::OnOpenDoor_Implementation()
{
    // C++ 동작이 필요하면 작성
}
