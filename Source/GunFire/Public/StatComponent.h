#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDamagedSignature, float, CurrentHealth, float, MaxHealth, float, ActualDamage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FHealedSignature, float, CurrentHealth, float, MaxHealth, float, HealAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeadSignature, AController*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStaminaComsumeSignature, float, CurrentStamina, float, MaxStamina);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUNFIRE_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStatComponent();

    bool TryConsumeStamina(float Cost);
    void ChangeSpeed(float Speed);
    void Heal(float Amount);

    float GetMovementSpeed(bool bIsSprint) const;
    bool IsDead() const;

public:
    // 델리게이트 객체
    UPROPERTY(BlueprintAssignable)
    FDamagedSignature OnDamaged;

    UPROPERTY(BlueprintAssignable)
    FHealedSignature OnHealed;

    UPROPERTY(BlueprintAssignable)
    FDeadSignature OnDead;

    UPROPERTY(BlueprintAssignable)
    FStaminaComsumeSignature OnStaminaConsumed;

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void TakeDamage(AActor* DamagedActor,
        float Damage,
        const UDamageType* DamageType,
        AController* Instigator,
        AActor* Causer
        );

protected:
    // 체력
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
    float CurrentHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
    float MaxHealth;

    // 공격력
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
    int32 AttackPower;

    // 방어력
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
    int32 Defense;

    // 걷기 속도, Base 속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
    float WalkSpeed;

    // 걷기 속도에 곱해서 달리기 속도 구하는 용도
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status")
    float SprintMultiplier;

    // 달리기 속도
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
    float SprintSpeed;

    // 스태미너
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
    float CurrentStamina;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
    float MaxStamina;

    // 초당 스태미너 회복량
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
    float StaminaRegen;

    // 스태미너 소모하는 액터, 아닌 액터 구분
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status")
    bool bUseStamina;

private:
    void RegenerateStamina();

    FTimerHandle StaminaRegenTimerHandle;
    float StaminaRegenInterval;
};
