#include "UI/DamagePopupComponent.h"

#include "StatComponent.h"

UDamagePopupComponent::UDamagePopupComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

    SpawnOffset = FVector(0.f, 0.f, 100.f);
    RandomOffsetRadius = 80.f;
    LastPopupLocation = FVector::ZeroVector;
    NewPositionTime = 3.f;
}

void UDamagePopupComponent::BeginPlay()
{
	Super::BeginPlay();

    AActor* Owner = GetOwner();
    if (!Owner) return;

    UStatComponent* StatComponent = Owner->FindComponentByClass<UStatComponent>();
    if (IsValid(StatComponent))
    {
        StatComponent->OnDamaged.AddDynamic(this, &UDamagePopupComponent::HandleDamaged);
    }
}

void UDamagePopupComponent::HandleDamaged(float ReceivedDamage, AController* DamageInstigator)
{
    ReceivedDamage = FMath::Max(ReceivedDamage, 0.0f);

    AActor* Owner = GetOwner();
    if (!Owner) return;

    UWorld* World = GetWorld();
    if (!World) return;

    // 일정 시간이 지나 위치를 새로 잡아야함
    // 타이머가 살아있다면 이전에 스폰된 위치 그대로 사용
    if (!World->GetTimerManager().IsTimerActive(PopupTimerHandle))
    {
        // X, Y 랜덤 위치 설정
        const FVector RandomOffset = FVector(
            FMath::RandRange(-RandomOffsetRadius, RandomOffsetRadius),
            FMath::RandRange(-RandomOffsetRadius, RandomOffsetRadius),
            0.f
            );

        // 스폰될 위치 지정
        LastPopupLocation = Owner->GetActorLocation() + SpawnOffset + RandomOffset;
    }

    World->GetTimerManager().ClearTimer(PopupTimerHandle);
    World->GetTimerManager().SetTimer(
        PopupTimerHandle,
        this,
        &UDamagePopupComponent::ClearLastLocation,
        NewPositionTime,
        false
        );

    ShowDamagePopup(ReceivedDamage, DamageInstigator, LastPopupLocation);
}

void UDamagePopupComponent::ClearLastLocation()
{
    LastPopupLocation = FVector::ZeroVector;
}

