#include "StatComponent.h"

UStatComponent::UStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

    StaminaRegenInterval = 0.1f;
    StaminaRegenDelayTime = 2.f;
    bUseStamina = false;
    bInvincible = false;

    constexpr float DefaultStats[] = {100.f, 20.f, 5.f, 100.f, 30.f};
    BaseStats.Initialize(DefaultStats);
}

void UStatComponent::BeginPlay()
{
    Super::BeginPlay();

    CalculateFinalStats();
    CurrentHealth = GetMaxHealth();
    CurrentStamina = GetMaxStamina();

    if (AActor* Owner = GetOwner())
    {
        Owner->OnTakeAnyDamage.AddDynamic(this, &UStatComponent::TakeDamage);
    }
}

void UStatComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(StaminaRegenTimerHandle);
        World->GetTimerManager().ClearTimer(RegenDelayTimerHandle);
    }

    Super::EndPlay(EndPlayReason);
}

bool UStatComponent::CanConsumeStamina(float Cost) const
{
    // 스태미너 소모하지 않거나 코스트가 0이라면 true 리턴
    if (!bUseStamina || Cost <= 0.f) return true;

    return CurrentStamina >= Cost;
}

bool UStatComponent::TryConsumeStamina(float Cost)
{
    // 스태미너 소모하지 않거나 코스트가 0이라면 아래 스태미너 소모 처리 X
    if (!bUseStamina || Cost <= 0.f) return true;

    if (CurrentStamina <= Cost) return false;

    UWorld* World = GetWorld();
    if (!World) return false;

    CurrentStamina -= Cost;

    // 스태미너 변경 이벤트 발생
    OnStaminaChanged.Broadcast(CurrentStamina, GetMaxStamina());

    // 기존에 일정시간 뒤에 스태미너 회복 타이머가 있다면 제거함
    if (World->GetTimerManager().IsTimerActive(RegenDelayTimerHandle))
    {
        World->GetTimerManager().ClearTimer(RegenDelayTimerHandle);
    }

    // 기존에 스태미너 회복 타이머가 가동중이라면 제거함
    if (World->GetTimerManager().IsTimerActive(StaminaRegenTimerHandle))
    {
        World->GetTimerManager().ClearTimer(StaminaRegenTimerHandle);
    }

    // 일정 시간 뒤에 스태미너 회복 하도록 타이머 세팅
    World->GetTimerManager().SetTimer(
        RegenDelayTimerHandle,
        this,
        &UStatComponent::StartRegenStamina,
        StaminaRegenDelayTime,
        false
        );


    return true;
}

void UStatComponent::RecoverStaminaMax()
{
    CurrentStamina = GetMaxStamina();
}

void UStatComponent::Heal(float Amount)
{
    if (IsDead() || Amount <= 0.f) return;

    // 오버힐 방지
    const float ActualAmount = CurrentHealth + Amount > GetMaxHealth() ? GetMaxHealth() - CurrentHealth : Amount;
    CurrentHealth += ActualAmount;

    // 회복 이벤트, 체력 변경 이벤트 발생
    OnHealed.Broadcast(ActualAmount);
    OnHealthChanged.Broadcast(CurrentHealth, GetMaxHealth());
}

void UStatComponent::AddBaseStat(ECombatStatType StatType, float AddValue)
{
    if (StatType == ECombatStatType::Count) return;

    BaseStats.AddValue(StatType, AddValue);
    CalculateFinalStats();
}

// 단일 모디파이어 추가
void UStatComponent::AddModifier(const FStatModifier& Modifier)
{
    if (Modifier.StatType == ECombatStatType::Count) return;

    Modifiers.Add(Modifier);
    CalculateFinalStats();
}

// 여러 타입의 모디파이어 한번에 추가
void UStatComponent::AddModifier(FName SourceID, TConstArrayView<ECombatStatType> StatTypes,
    EStatModifierType ModifierType, float Value)
{
    for (const ECombatStatType StatType : StatTypes)
    {
        if (StatType == ECombatStatType::Count) continue;

        FStatModifier Modifier;
        Modifier.SourceID = SourceID;
        Modifier.StatType = StatType;
        Modifier.ModifierType = ModifierType;
        Modifier.Value = Value;

        Modifiers.Add(Modifier);
    }

    CalculateFinalStats();
}

void UStatComponent::RemoveModifier(FName SourceID)
{
    Modifiers.RemoveAll([SourceID](const FStatModifier& Modifier)
    {
        return Modifier.SourceID == SourceID;
    });

    CalculateFinalStats();
}

void UStatComponent::ResetModifier()
{
    Modifiers.Empty();
    CalculateFinalStats();
}

void UStatComponent::CalculateFinalStats()
{
    FCombatStat AddStats;
    FCombatStat MultiplyStats(1.f);

    // 기존에 최대체력, 최대 스태미너였는지 확인
    bool bIsMaxHealth = FMath::IsNearlyEqual(CurrentHealth, GetMaxHealth());
    bool bIsMaxStamina = FMath::IsNearlyEqual(CurrentStamina, GetMaxStamina());

    for (const FStatModifier& Modifier : Modifiers)
    {
        if (Modifier.ModifierType == EStatModifierType::Add)
        {
            AddStats.AddValue(Modifier.StatType, Modifier.Value);
        }
        else
        {
            MultiplyStats.AddValue(Modifier.StatType, Modifier.Value);
        }
    }

    FinalStats = (BaseStats + AddStats) * MultiplyStats;

    // 기존에 체력이 가득 찼다면 최대체력을 그대로 따라감
    // 아니라면 보너스가 감소할 수 있으므로 0 ~ MaxHealth 로 조절 시도
    CurrentHealth = bIsMaxHealth
        ? GetMaxHealth() : FMath::Clamp(CurrentHealth, 0, GetMaxHealth());
    CurrentStamina = bIsMaxStamina
        ? GetMaxStamina() : FMath::Clamp(CurrentStamina, 0, GetMaxStamina());

    // 체력이나 스태미너 갱신됐을 수 있으므로 이벤트 발생시켜야 함
    OnHealthChanged.Broadcast(CurrentHealth, GetMaxHealth());
    OnStaminaChanged.Broadcast(CurrentStamina, GetMaxStamina());
}

float UStatComponent::GetStatValue(ECombatStatType StatType) const
{
    return FinalStats.GetValue(StatType);
}

bool UStatComponent::IsDead() const
{
    return CurrentHealth <= 0.f;
}

float UStatComponent::GetMaxHealth() const
{
    return GetStatValue(ECombatStatType::MaxHealth);
}

float UStatComponent::GetCurrentHealth() const
{
    return CurrentHealth;
}

float UStatComponent::GetAttackPower() const
{
    return GetStatValue(ECombatStatType::AttackPower);
}

float UStatComponent::GetDefense() const
{
    return GetStatValue(ECombatStatType::Defense);
}

float UStatComponent::GetMaxStamina() const
{
    return GetStatValue(ECombatStatType::MaxStamina);
}

float UStatComponent::GetCurrentStamina() const
{
    return CurrentStamina;
}

const FCombatStat& UStatComponent::GetBaseStats() const
{
    return BaseStats;
}

bool UStatComponent::IsInvincible() const
{
    return bInvincible;
}

void UStatComponent::SetBaseStats(const FCombatStat& NewStats)
{
    BaseStats = NewStats;
}

void UStatComponent::SetCurrentHealth(float NewHealth)
{
    CurrentHealth = NewHealth;
}

void UStatComponent::SetInvincible(bool bIsInvincible)
{
    bInvincible = bIsInvincible;
}

void UStatComponent::TakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                AController* Instigator, AActor* Causer)
{
    // 무적상태면 건너뛰기
    if (bInvincible) return;

    if (IsDead() || Damage <= 0.f) return;

    const float BaseDamage = Damage - GetDefense();
    const double DamageRange = FMath::RandRange(0.95, 1.05);

    const float ActualDamage = FMath::Max(BaseDamage * DamageRange, 0);

    // 데미지는 초과 분량 그대로 반영하면서 체력만 0으로 조정
    CurrentHealth -= ActualDamage;
    CurrentHealth = FMath::Max(0.f, CurrentHealth);

    // 피격 처리 이벤트, 체력 변경 이벤트 발생
    OnDamaged.Broadcast(ActualDamage, Instigator);
    OnHealthChanged.Broadcast(CurrentHealth, GetMaxHealth());

    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("피해 적용 완료! 남은 체력: %.1f"), CurrentHealth));

    if (IsDead())
    {
        UE_LOG(LogTemp, Warning, TEXT("%s 사망"), *this->GetName());
        // 사망 이벤트 발생
        OnDead.Broadcast(Instigator);
    }
}

// 행동 이후 일정시간 뒤에 스태미너 회복을 발생시키는 함수
void UStatComponent::StartRegenStamina()
{
    UWorld* World = GetWorld();
    if (!World) return;

    World->GetTimerManager().ClearTimer(RegenDelayTimerHandle);

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
}

// 스태미너 회복 함수
void UStatComponent::RegenerateStamina()
{
    if (!bUseStamina) return;

    CurrentStamina += GetStatValue(ECombatStatType::StaminaRegen) * StaminaRegenInterval;
    CurrentStamina = FMath::Min(CurrentStamina, GetMaxStamina());

    // 스태미너 변경 이벤트 발생
    OnStaminaChanged.Broadcast(CurrentStamina, GetMaxStamina());

    // 스태미너가 가득차면 회복 타이머 종료
    if (CurrentStamina >= GetMaxStamina())
    {
        UWorld* World = GetWorld();
        if (!World) return;

        World->GetTimerManager().ClearTimer(StaminaRegenTimerHandle);
    }
}

