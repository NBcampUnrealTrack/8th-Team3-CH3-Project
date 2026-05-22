#include "InventoryComponent.h"
#include "ItemSystemTypes.h"
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

    // 행 이름 가져오기
    TArray<FName> RowNames = PassiveItemTable->GetRowNames();
    if (RowNames.Num() == 0) return Result;

    TArray<FGF_PassiveItemData> ValidOptions;

    // 데이터 검색
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
int32 UInventoryComponent::UpgradeItem(int32 TargetIndex, int32 MaterialIndex)
{
    UE_LOG(LogTemp, Warning, TEXT("Upgrade Called!"));

    // 인덱스 체크
    if (!OwnedPassives.IsValidIndex(TargetIndex) ||
        !OwnedPassives.IsValidIndex(MaterialIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid Index"));
        return -1;
    }

    // 데이터 테이블 체크
    if (!PassiveItemTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("PassiveItemTable Null"));
        return -1;
    }

    // 강화 대상
    FGF_PassiveItemData& TargetItem = OwnedPassives[TargetIndex];

    // Row 찾기
    FGF_PassiveItemData* RowData =
        PassiveItemTable->FindRow<FGF_PassiveItemData>(
            TargetItem.ItemRowName,
            TEXT("UpgradeItem")
        );

    if (!RowData)
    {
        UE_LOG(LogTemp, Warning, TEXT("Row Not Found"));
        return -1;
    }

    // =========================
    // 강화 처리
    // =========================

    // 이름 강제 변경
    TargetItem.ItemName = RowData->UpgradeItemName;

    // 설명 강제 변경
    TargetItem.ItemDescription = RowData->UpgradeItemDescription;

    // 스탯 증가
    TargetItem.StatValue += RowData->StatValue;

    // 레벨 강제 설정
    TargetItem.CurrentLevel = 1;

    UE_LOG(LogTemp, Warning,
        TEXT("Upgrade Success : %s / Level : %d"),
        *TargetItem.ItemName.ToString(),
        TargetItem.CurrentLevel
    );

    // =========================
    // 재료 소모
    // =========================

    if (OwnedPassives[MaterialIndex].StackCount > 1)
    {
        OwnedPassives[MaterialIndex].StackCount--;
    }
    else
    {
        OwnedPassives[MaterialIndex] = FGF_PassiveItemData();
    }

    // =========================
    // UI 갱신 알림
    // =========================

    OnInventoryChanged.Broadcast();

    return TargetIndex;
}
