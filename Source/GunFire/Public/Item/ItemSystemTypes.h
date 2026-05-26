#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemSystemTypes.generated.h"

// 아이템 타입 (기존 유지)
UENUM(BlueprintType)
enum class EGF_ItemType : uint8
{
    Passive   UMETA(DisplayName = "Passive"),
    Active    UMETA(DisplayName = "Active"),
    Material  UMETA(DisplayName = "Material")
};

// 아이템 공통 (기존 유지)
USTRUCT(BlueprintType)
struct FGF_ItemBase : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FName ItemRowName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FText ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (MultiLine = true))
    FText ItemDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FText UpgradeItemName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (MultiLine = true))
    FText UpgradeItemDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    class UTexture2D* ItemIcon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    EGF_ItemType ItemType = EGF_ItemType::Passive;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 CurrentLevel = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 StackCount = 1;
};

// 패시브 아이템
USTRUCT(BlueprintType)
struct FGF_PassiveItemData : public FGF_ItemBase
{
    GENERATED_BODY()

    // 기본 획득 시 % 수치 0.1 입력 시 10% 증가로 기획
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive | Base Stat %")
    float AttackPower_Pct = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive | Base Stat %")
    float MaxHealth_Pct = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive | Base Stat %")
    float Defense_Pct = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive | Base Stat %")
    float MaxStamina_Pct = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive | Base Stat %")
    float StaminaRegen_Pct = 0.0f;

    // 업그레이드 시 % 수치
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive | Upgrade Stat %")
    float Upgrade_AttackPower_Pct = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive | Upgrade Stat %")
    float Upgrade_MaxHealth_Pct = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive | Upgrade Stat %")
    float Upgrade_Defense_Pct = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive | Upgrade Stat %")
    float Upgrade_MaxStamina_Pct = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive | Upgrade Stat %")
    float Upgrade_StaminaRegen_Pct = 0.0f;
};

// 액티브 아이템 (기존 유지)
USTRUCT(BlueprintType)
struct FGF_ActiveItemData : public FGF_ItemBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Active")
    float Cooldown = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Active")
    float Duration = 0.0f;
};
