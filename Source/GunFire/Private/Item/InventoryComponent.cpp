#include "Item/InventoryComponent.h"
#include "Engine/DataTable.h"
#include "Game/SessionData.h"

UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

// 패시브 옵션 가져오기
TArray<FGF_PassiveItemData> UInventoryComponent::GetRandomPassiveOptions(int32 Count)
{
    TArray<FGF_PassiveItemData> Result;
    if (!PassiveItemTable) return Result;

    TArray<FGF_PassiveItemData*> AllRows;
    PassiveItemTable->GetAllRows<FGF_PassiveItemData>(TEXT("GetAllRows"), AllRows);
    if (AllRows.Num() == 0) return Result;

    TArray<FGF_PassiveItemData> ValidOptions;
    for (auto RowPtr : AllRows)
    {
        if (RowPtr)
        {
            bool bAlreadyOwned = false;
            for (const FGF_PassiveItemData& OwnedItem : OwnedPassives)
            {
                if (RowPtr->ItemName.EqualTo(OwnedItem.ItemName))
                {
                    bAlreadyOwned = true;
                    break;
                }
            }
            if (!bAlreadyOwned) ValidOptions.Add(*RowPtr);
        }
    }

    if (ValidOptions.Num() == 0) return Result;

    for (int32 i = ValidOptions.Num() - 1; i > 0; --i)
    {
        int32 j = FMath::RandRange(0, i);
        ValidOptions.Swap(i, j);
    }

    int32 FinalCount = FMath::Min(Count, ValidOptions.Num());
    for (int32 i = 0; i < FinalCount; ++i) Result.Add(ValidOptions[i]);

    return Result;
}

// 액티브 옵션 가져오기
TArray<FGF_ActiveItemData> UInventoryComponent::GetRandomActiveOptions(int32 Count)
{
    TArray<FGF_ActiveItemData> Result;
    if (!ActiveItemTable) return Result;

    TArray<FGF_ActiveItemData*> AllRows;
    ActiveItemTable->GetAllRows<FGF_ActiveItemData>(TEXT("GetAllRows"), AllRows);
    if (AllRows.Num() == 0) return Result;

    TArray<FGF_ActiveItemData> ValidOptions;
    for (auto RowPtr : AllRows)
    {
        if (RowPtr)
        {
            bool bAlreadyOwned = false;
            for (const FGF_ActiveItemData& OwnedItem : OwnedActives)
            {
                if (RowPtr->ItemName.EqualTo(OwnedItem.ItemName))
                {
                    bAlreadyOwned = true;
                    break;
                }
            }
            if (!bAlreadyOwned) ValidOptions.Add(*RowPtr);
        }
    }

    if (ValidOptions.Num() == 0) return Result;

    for (int32 i = ValidOptions.Num() - 1; i > 0; --i)
    {
        int32 j = FMath::RandRange(0, i);
        ValidOptions.Swap(i, j);
    }

    int32 FinalCount = FMath::Min(Count, ValidOptions.Num());
    for (int32 i = 0; i < FinalCount; ++i) Result.Add(ValidOptions[i]);

    return Result;
}

// 패시브 추가
void UInventoryComponent::AddPassive(FGF_PassiveItemData NewData)
{
    bool bFound = false;

    for (FGF_PassiveItemData& OwnedItem : OwnedPassives)
    {
        if (NewData.ItemName.EqualTo(OwnedItem.ItemName))
        {
            OwnedItem.StackCount += NewData.StackCount;
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        OwnedPassives.Add(NewData);
    }
}

// 액티브 추가
void UInventoryComponent::AddActive(FGF_ActiveItemData NewData)
{
    bool bFound = false;

    for (FGF_ActiveItemData& OwnedItem : OwnedActives)
    {
        if (NewData.ItemName.EqualTo(OwnedItem.ItemName))
        {
            OwnedItem.StackCount += NewData.StackCount;
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        OwnedActives.Add(NewData);
    }
}

// 재료 추가
void UInventoryComponent::AddMaterial(FGF_PassiveItemData NewData)
{
    bool bFound = false;

    for (FGF_PassiveItemData& OwnedItem : OwnedMaterials)
    {
        if (NewData.ItemName.EqualTo(OwnedItem.ItemName))
        {
            OwnedItem.StackCount += NewData.StackCount;
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        OwnedMaterials.Add(NewData);
    }
}

void UInventoryComponent::SetInventorySessionData(const FInventorySessionData& InventorySessionData)
{
    OwnedPassives = InventorySessionData.OwnedPassives;
    OwnedActives = InventorySessionData.OwnedActives;
    OwnedMaterials = InventorySessionData.OwnedMaterials;
}
