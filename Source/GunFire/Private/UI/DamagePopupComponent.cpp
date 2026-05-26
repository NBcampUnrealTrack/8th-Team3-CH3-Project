#include "UI/DamagePopupComponent.h"

#include "StatComponent.h"

UDamagePopupComponent::UDamagePopupComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

    SpawnOffset = FVector(0.f, 0.f, 100.f);
    RandomOffsetRadius = 20.f;
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

    // X, Y 랜덤 위치 설정
    const FVector RandomOffset = FVector(
        FMath::RandRange(-RandomOffsetRadius, RandomOffsetRadius),
        FMath::RandRange(-RandomOffsetRadius, RandomOffsetRadius),
        0.f
        );

    // 스폰될 위치 지정
    const FVector SpawnLocation = Owner->GetActorLocation() + SpawnOffset + RandomOffset;

    ShowDamagePopup(ReceivedDamage, DamageInstigator, SpawnLocation);
}

