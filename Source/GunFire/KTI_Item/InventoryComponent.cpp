#include "InventoryComponent.h"

UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

TArray<FGF_PassiveItemData> UInventoryComponent::GetRandomPassiveOptions(int32 Count)
{
    TArray<FGF_PassiveItemData> Result;
    if (!PassiveItemTable) return Result;

    TArray<FName> RowNames = PassiveItemTable->GetRowNames();
    if (RowNames.Num() == 0) return Result;

    TArray<FGF_PassiveItemData> ValidOptions;

    for (const FName& Name : RowNames)
    {
        FGF_PassiveItemData* RowPtr = PassiveItemTable->FindRow<FGF_PassiveItemData>(Name, TEXT("GetRandomPassiveOptions"));
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
            if (!bAlreadyOwned)
            {
                FGF_PassiveItemData CopyData = *RowPtr;
                CopyData.ItemRowName = Name;
                ValidOptions.Add(CopyData);
            }
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
    bool bFound = false;

    for (FGF_PassiveItemData& OwnedItem : OwnedPassives)
    {
        if (NewData.ItemName.EqualTo(OwnedItem.ItemName))
        {
            OwnedItem.StackCount += NewData.StackCount;

            if (OwnedItem.ItemRowName.IsNone() || OwnedItem.ItemRowName == TEXT("None"))
            {
                OwnedItem.ItemRowName = NewData.ItemRowName;
            }

            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        OwnedPassives.Add(NewData);
    }
}

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
bool UInventoryComponent::UpgradeItem(int32 TargetIndex)
{
    if (!OwnedPassives.IsValidIndex(TargetIndex)) return false;
    if (!PassiveItemTable) return false;
    if (OwnedPassives[TargetIndex].ItemRowName.IsNone()) return false;

    FGF_PassiveItemData* RowData = PassiveItemTable->FindRow<FGF_PassiveItemData>(OwnedPassives[TargetIndex].ItemRowName, TEXT("UpgradeItem"));
    if (!RowData) return false;

    // 데이터 적용
    OwnedPassives[TargetIndex].ItemName = RowData->UpgradeItemName;
    OwnedPassives[TargetIndex].ItemDescription = RowData->UpgradeItemDescription;
    OwnedPassives[TargetIndex].ItemIcon = RowData->ItemIcon;
    OwnedPassives[TargetIndex].StatValue += RowData->StatValue;
    OwnedPassives[TargetIndex].CurrentLevel += 1;

    OnInventoryChanged.Broadcast();
    return true;
}

// 재료 소모 
bool UInventoryComponent::ConsumeMaterial(int32 DummyIndex)
{
    int32 FoundMaterialIndex = INDEX_NONE;

    // 1. 큰 바구니(OwnedPassives)를 돌며 아이템 타입을 검사합니다.
    for (int32 i = 0; i < OwnedPassives.Num(); ++i)
    {
        // 변수 타입이 Enum이므로 열거형 값인 EGF_ItemType::Material 하고만 비교해야 합니다.
        if (OwnedPassives[i].ItemType == EGF_ItemType::Material)
        {
            FoundMaterialIndex = i;
            break;
        }
    }

    // 2. 만약 인벤토리에 재료가 하나도 없다면 실패 처리
    if (FoundMaterialIndex == INDEX_NONE) return false;

    // 3. 찾은 재료 인덱스를 기준으로 수량을 깎거나 삭제합니다.
    if (OwnedPassives[FoundMaterialIndex].StackCount > 1)
    {
        OwnedPassives[FoundMaterialIndex].StackCount--;
    }
    else
    {
        OwnedPassives.RemoveAt(FoundMaterialIndex);
    }

    // 4. UI에 변경 신호 발송
    OnInventoryChanged.Broadcast();
    return true;
}
