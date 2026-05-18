#include "InventoryComponent.h"
#include "Engine/DataTable.h" 

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

// 아이템 강화
void UInventoryComponent::UpgradeItem(int32 TargetIndex, int32 MaterialIndex, bool bSuccess)
{
    if (!OwnedPassives.IsValidIndex(TargetIndex) || !OwnedMaterials.IsValidIndex(MaterialIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("Upgrade Item Failed: Invalid Passive or Material Index."));
        return;
    }

    OwnedMaterials[MaterialIndex].StackCount--;

    if (OwnedMaterials[MaterialIndex].StackCount <= 0)
    {
        OwnedMaterials.RemoveAt(MaterialIndex);
    }

    if (bSuccess)
    {
        // 아이템 레벨 증가
        OwnedPassives[TargetIndex].CurrentLevel++;

        OwnedPassives[TargetIndex].ItemName = OwnedPassives[TargetIndex].UpgradeItemName;
        OwnedPassives[TargetIndex].ItemDescription = OwnedPassives[TargetIndex].UpgradeItemDescription;

        if (PassiveItemTable)
        {
            FString RowString = FString::Printf(TEXT("%s_%d"), *OwnedPassives[TargetIndex].UpgradeItemName.ToString(), OwnedPassives[TargetIndex].CurrentLevel + 1);
            FName NextUpgradeRowName = FName(*RowString);

            FGF_PassiveItemData* NextRowData = PassiveItemTable->FindRow<FGF_PassiveItemData>(NextUpgradeRowName, TEXT("FindNextUpgradeData"));
            if (NextRowData)
            {
                OwnedPassives[TargetIndex].UpgradeItemName = NextRowData->UpgradeItemName;
                OwnedPassives[TargetIndex].UpgradeItemDescription = NextRowData->UpgradeItemDescription;

                OwnedPassives[TargetIndex].StatValue = NextRowData->StatValue;
            }
        }
    }

    if (OnInventoryChanged.IsBound())
    {
        OnInventoryChanged.Broadcast();
    }
}
