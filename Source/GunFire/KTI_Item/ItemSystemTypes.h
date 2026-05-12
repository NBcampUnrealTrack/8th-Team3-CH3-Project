#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemSystemTypes.generated.h"

// 아이템 타입 
UENUM(BlueprintType)
enum class EGF_ItemType : uint8
{
    Passive   UMETA(DisplayName = "Passive"),
    Active    UMETA(DisplayName = "Active"),
    Material  UMETA(DisplayName = "Material")
};

// 아이템 공통
USTRUCT(BlueprintType)
struct FGF_ItemBase : public FTableRowBase
{
    GENERATED_BODY()

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
};

// 패시브 아이템 
USTRUCT(BlueprintType)
struct FGF_PassiveItemData : public FGF_ItemBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive")
    FName StatTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive")
    float StatValue = 0.0f;
};

// 액티브 아이템 
USTRUCT(BlueprintType)
struct FGF_ActiveItemData : public FGF_ItemBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Active")
    float Cooldown = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Active")
    float Duration = 0.0f;
};
