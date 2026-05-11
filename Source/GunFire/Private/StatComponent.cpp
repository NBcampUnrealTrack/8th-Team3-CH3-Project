#include "StatComponent.h"

UStatComponent::UStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

    MaxHealth = 100.f;
    AttackPower = 20.f;
    Defense = 5.f;
    WalkSpeed = 600.f;
    SprintMultiplier = 1.5f;
    MaxStamina = 100.f;
    StaminaRegen = 30.f;
    StaminaRegenInterval = 0.1f;
}

void UStatComponent::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth;
    SprintSpeed = WalkSpeed * SprintMultiplier;
    CurrentStamina = MaxStamina;

    if (AActor* Owner = GetOwner())
    {
        Owner->OnTakeAnyDamage.AddDynamic(this, &UStatComponent::TakeDamage);
    }
}

bool UStatComponent::TryConsumeStamina(float Cost)
{
    // 스태미너 소모하지 않는 액터면 그냥 true 리턴
    if (!bUseStamina) return true;

    UWorld* World = GetWorld();
    if (!World) return false;

    if (CurrentStamina < Cost) return false;

    CurrentStamina -= Cost;
    OnStaminaConsumed.Broadcast(CurrentStamina, MaxStamina);

    // 타이머가 세팅되어 있지 않다면 스태미너 회복 타이머 세팅
    if (!World->GetTimerManager().IsTimerActive(StaminaRegenTimerHandle))
    {
        World->GetTimerManager().SetTimer(
            StaminaRegenTimerHandle,
            this,
            &UStatComponent::RegenerateStamina,
            StaminaRegenInterval,
            true
            );
    }
    
    return true;
}

void UStatComponent::ChangeSpeed(float Speed)
{
    WalkSpeed = Speed;
    SprintSpeed = WalkSpeed * SprintMultiplier;
}

void UStatComponent::Heal(float Amount)
{
    if (IsDead() || Amount <= 0.f) return;

    // 오버힐 방지
    const float ActualAmount = (CurrentHealth + Amount > MaxHealth) ? MaxHealth - CurrentHealth : Amount;
    CurrentHealth += ActualAmount;

    OnHealed.Broadcast(CurrentHealth, MaxHealth, ActualAmount);
}

float UStatComponent::GetMovementSpeed(bool bIsSprint) const
{
    return bIsSprint ? SprintSpeed : WalkSpeed;
}

bool UStatComponent::IsDead() const
{
    return CurrentHealth <= 0.f;
}

void UStatComponent::TakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
    AController* Instigator, AActor* Causer)
{
    if (IsDead() || Damage <= 0.f) return;

    const float BaseDamage = Damage - Defense;
    const double DamageRange = FMath::RandRange(0.95, 1.05);

    const float ActualDamage = FMath::Max(BaseDamage * DamageRange, 0);

    // 데미지는 초과 분량 그대로 반영하면서 체력만 0으로 조정
    CurrentHealth -= ActualDamage;
    CurrentHealth = FMath::Max(0.f, CurrentHealth);

    OnDamaged.Broadcast(CurrentHealth, MaxHealth, ActualDamage);
    if (IsDead())
    {
        OnDead.Broadcast(Instigator);
    }
}

void UStatComponent::RegenerateStamina()
{
    CurrentStamina += StaminaRegen * StaminaRegenInterval;
    CurrentStamina = FMath::Min(CurrentStamina, MaxStamina);

    // 스태미너가 가득차면 회복 타이머 종료
    if (CurrentStamina >= MaxStamina)
    {
        UWorld* World = GetWorld();
        if (!World) return;

        World->GetTimerManager().ClearTimer(StaminaRegenTimerHandle);
    }
}

