#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatStat.h"
#include "StatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDamagedSignature, float, ActualDamage, AController*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHealedSignature, float, HealAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeadSignature, AController*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHealthChangedSignature, float, CurrentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStaminaChangedSignature, float, CurrentStamina, float, MaxStamina);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUNFIRE_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStatComponent();

    bool TryConsumeStamina(float Cost);
    void Heal(float Amount);
    void AddBaseStat(ECombatStatType StatType, float AddValue);
    void AddModifier(const FStatModifier& Modifier);
    void RemoveModifier(FName SourceID);
    void CalculateFinalStats();

    float GetStatValue(ECombatStatType StatType) const;
    bool IsDead() const;

    float GetMaxHealth() const;
    float GetCurrentHealth() const;
    float GetAttackPower() const;
    float GetDefense() const;
    float GetMovementSpeed(bool bIsSprint) const;
    float GetMaxStamina() const;

public:
    // 델리게이트
    UPROPERTY(BlueprintAssignable)
    FDamagedSignature OnDamaged;

    UPROPERTY(BlueprintAssignable)
    FHealedSignature OnHealed;

    UPROPERTY(BlueprintAssignable)
    FHealthChangedSignature OnHealthChanged;

    UPROPERTY(BlueprintAssignable)
    FDeadSignature OnDead;

    UPROPERTY(BlueprintAssignable)
    FStaminaChangedSignature OnStaminaChanged;

protected:
    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UFUNCTION()
    void TakeDamage(AActor* DamagedActor,
        float Damage,
        const UDamageType* DamageType,
        AController* Instigator,
        AActor* Causer
        );

protected:
    // 기본 스탯
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
    FCombatStat BaseStats;

    // 최종 스탯
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
    FCombatStat FinalStats;

    // 유물이나 장비로 인한 변화값들을 저장
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
    TArray<FStatModifier> Modifiers;

    // 체력
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stat")
    float CurrentHealth;

    // 스태미너
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stat")
    float CurrentStamina;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat")
    bool bUseStamina;

private:
    void StartRegenStamina();
    void RegenerateStamina();

    // 스태미너 회복 타이머, 액션 이후 일정 시간 뒤에 스태미너 회복을 발생시킬 타이머
    FTimerHandle StaminaRegenTimerHandle;
    FTimerHandle RegenDelayTimerHandle;
    float StaminaRegenInterval;
    float StaminaRegenDelayTime;
};
