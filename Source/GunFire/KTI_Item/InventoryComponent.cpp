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
    UE_LOG(LogTemp, Warning, TEXT("Upgrade Called! TargetIndex: %d"), TargetIndex);

    // 1. 방어 코드: 인덱스가 현재 패시브 배열 범위 내에 있는지 확인
    if (!OwnedPassives.IsValidIndex(TargetIndex) || !OwnedPassives.IsValidIndex(MaterialIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("Upgrade Item Failed: Invalid Passive or Material Index."));
        return -1;
    }

    if (!PassiveItemTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("Upgrade Item Failed: PassiveItemTable is Null."));
        return -1;
    }

    // 2. 강화 대상의 데이터 테이블 RowKey 확인
    FName RowKey = OwnedPassives[TargetIndex].ItemRowName;
    if (RowKey.IsNone() || RowKey == TEXT("None"))
    {
        UE_LOG(LogTemp, Warning, TEXT("Upgrade Item Failed: ItemRowName is None."));
        return -1;
    }

    // 3. 데이터 테이블에서 정보 검색
    FGF_PassiveItemData* TargetRowData = PassiveItemTable->FindRow<FGF_PassiveItemData>(RowKey, TEXT("UpgradeItem"));

    if (TargetRowData)
    {
        // 4. 배열 아이템 데이터 수정
        OwnedPassives[TargetIndex].CurrentLevel++;
        OwnedPassives[TargetIndex].ItemName = TargetRowData->UpgradeItemName;
        OwnedPassives[TargetIndex].ItemDescription = TargetRowData->UpgradeItemDescription;
        OwnedPassives[TargetIndex].StatValue += TargetRowData->StatValue;

        UE_LOG(LogTemp, Warning, TEXT("!!! Upgrade Success !!! Item: %s, Level: %d"),
            *OwnedPassives[TargetIndex].ItemName.ToString(), OwnedPassives[TargetIndex].CurrentLevel);

        // 5. 재료 소모 처리 (수정됨!)
        if (OwnedPassives[MaterialIndex].StackCount > 1)
        {
            // 갯수가 여유 있으면 숫자만 감소
            OwnedPassives[MaterialIndex].StackCount--;
        }
        else
        {
            // [중요 수정] 배열에서 RemoveAt으로 삭제하지 않고, 
            // 빈 아이템 구조체로 덮어씌워 인덱스를 보존합니다.
            OwnedPassives[MaterialIndex] = FGF_PassiveItemData();

            // 만약 FPassiveItem() 기본 생성자가 모든 값을 초기화하지 않는다면 아래처럼 명시적으로 비우세요.
            // OwnedPassives[MaterialIndex].ItemRowName = FName("None");
            // OwnedPassives[MaterialIndex].StackCount = 0;
        }

        // 6. 인벤토리 변경 알림 브로드캐스트
        if (OnInventoryChanged.IsBound())
        {
            OnInventoryChanged.Broadcast();
        }

        return TargetIndex;
    }

    UE_LOG(LogTemp, Warning, TEXT("Upgrade Item Failed: Row [%s] not found in DataTable."), *RowKey.ToString());
    return -1;
}
