#pragma once

#include "CoreMinimal.h"
#include "CombatStat.generated.h"

// 스탯 타입
UENUM(BlueprintType)
enum class ECombatStatType : uint8
{
    MaxHealth           UMETA(DisplayName = "최대 체력"),
    AttackPower         UMETA(DisplayName = "공격력"),
    Defense             UMETA(DisplayName = "방어력"),
    WalkSpeed           UMETA(DisplayName = "기본 속도"),
    SprintMultiplier    UMETA(DisplayName = "달리기 속도 배율"),
    MaxStamina          UMETA(DisplayName = "최대 스태미너"),
    StaminaRegen        UMETA(DisplayName = "스태미너 회복량"),

    Count               UMETA(Hidden)
};

// Enum 범위 설정, 아래 TEnumRange<>() 를 사용하기 위함
ENUM_RANGE_BY_COUNT(ECombatStatType, ECombatStatType::Count);

// 스탯 구조체
USTRUCT(BlueprintType)
struct FCombatStat
{
    GENERATED_BODY()

    // 배열의 형태로 스탯 정보 저장
    // Enum 이랑 연결해서 사용하기 편리함
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
    TArray<float> Values;

    // 생성자
    FCombatStat() : FCombatStat(0.f) {}

    explicit FCombatStat(float Value)
    {
        Values.Init(Value, static_cast<int32>(ECombatStatType::Count));
    }

    // Enum 순서에 맞게 스탯을 초기화 하는 함수
    // 최대체력, 공격력, 방어력, 걷기속도, 달리기 배율, 최대스태미너, 스태미너 회복량...
    void Initialize(TArrayView<const float> StatView)
    {
        // Value 전체 초기화
        Values.Init(0.f, static_cast<int32>(ECombatStatType::Count));

        // Enum 값이 전부 안들어오거나 더 들어올 경우를 위한 Min 검사
        int32 Count = FMath::Min(Values.Num(), StatView.Num());

        for (int32 i = 0; i < Count; ++i)
        {
            Values[i] = StatView[i];
        }
    }

    FCombatStat operator+(const FCombatStat& Other) const
    {
        FCombatStat Result;
        // TEnumRange 로 Enum 순회 가능
        for (const auto& Type : TEnumRange<ECombatStatType>())
        {
            Result.SetValue(Type, GetValue(Type) + Other.GetValue(Type));
        }
        return Result;
    }

    FCombatStat operator*(const FCombatStat& Other) const
    {
        FCombatStat Result;
        for (const auto& Type : TEnumRange<ECombatStatType>())
        {
            Result.SetValue(Type, GetValue(Type) * Other.GetValue(Type));
        }
        return Result;
    }

    float GetValue(ECombatStatType StatType) const
    {
        int32 Index = static_cast<int32>(StatType);
        return Values.IsValidIndex(Index) ? Values[Index] : 0.f;
    }

    void SetValue(ECombatStatType StatType, float NewValue)
    {
        int32 Index = static_cast<int32>(StatType);
        if (Values.IsValidIndex(Index))
        {
            Values[Index] = NewValue;
        }
    }

    void AddValue(ECombatStatType StatType, float AddValue)
    {
        SetValue(StatType, GetValue(StatType) + AddValue);
    }
};

// 스탯 변경 합, 곱 구분
UENUM(BlueprintType)
enum class EStatModifierType : uint8
{
    Add,
    Multiply
};

// 변경할 스탯 구조체
USTRUCT(BlueprintType)
struct FStatModifier
{
    GENERATED_BODY()

    // 유물이나 장비 ID, 기존 유물 삭제 시 Name 사용
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
    FName SourceID = NAME_None;

    // 변경할 스탯 타입
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
    ECombatStatType StatType = ECombatStatType::Count;

    // 변경할 방식 (합 or 곱)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
    EStatModifierType ModifierType = EStatModifierType::Add;

    // 수치
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
    float Value = 0.f;
};
