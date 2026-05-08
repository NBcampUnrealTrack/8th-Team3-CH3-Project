#include "InventoryComponent.h"
#include "Engine/DataTable.h" 

UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    UpgradeMaterialCount = 0;
}

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


void UInventoryComponent::AddPassive(FGF_PassiveItemData NewData)
{
    /* for (const FGF_PassiveItemData& OwnedItem : OwnedPassives)
    {
        if (NewData.ItemName.EqualTo(OwnedItem.ItemName)) return;
    }
    */
    OwnedPassives.Add(NewData);
}

void UInventoryComponent::AddActive(FGF_ActiveItemData NewData)
{
    /*
    for (const FGF_ActiveItemData& OwnedItem : OwnedActives)
    {
        if (NewData.ItemName.EqualTo(OwnedItem.ItemName)) return;
    }
    */
    OwnedActives.Add(NewData);
}

void UInventoryComponent::AddMaterial(int32 Amount)
{
    UpgradeMaterialCount += Amount;
}
