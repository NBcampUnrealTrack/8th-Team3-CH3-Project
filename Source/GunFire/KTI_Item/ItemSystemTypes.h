#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemSystemTypes.generated.h"

// 아이템 타입 
UENUM(BlueprintType)
enum class EGF_ItemType : uint8
{
    Passive    UMETA(DisplayName = "Passive"),
    Active     UMETA(DisplayName = "Active"),
    Material   UMETA(DisplayName = "Material")
};

// 아이템 구조체
USTRUCT(BlueprintType)
struct FGF_ItemBase : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FText ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (MultiLine = true))
    FText ItemDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    EGF_ItemType ItemType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 CurrentLevel = 0;
};

// 패시브 구조체
USTRUCT(BlueprintType)
struct FGF_PassiveItemData : public FGF_ItemBase
{
    GENERATED_BODY()

    // 스탯 태그 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive")
    FName StatTag;

    // 수치
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive")
    float StatValue;
};

// 액티브 구조체 
USTRUCT(BlueprintType)
struct FGF_ActiveItemData : public FGF_ItemBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Active")
    float Cooldown;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Active")
    float Duration;
};
