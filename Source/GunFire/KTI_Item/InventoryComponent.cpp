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

    // 테이블 할당 체크
    if (!PassiveItemTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("패시브 할당 X"));
        return Result;
    }

    // 데이터 가져오기
    TArray<FGF_PassiveItemData*> AllRows;
    PassiveItemTable->GetAllRows<FGF_PassiveItemData>(TEXT("GetAllRows"), AllRows);

    if (AllRows.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("데이터 테이블 로드 실패: 구조체 타입 불일치 가능성"));
        return Result;
    }

    //  보유 중인 아이템 제외하고 리스트 
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

            if (!bAlreadyOwned)
            {
                ValidOptions.Add(*RowPtr);
            }
        }
    }

    if (ValidOptions.Num() == 0) return Result;

    // 랜덤 셔플 (중복 방지)
    for (int32 i = ValidOptions.Num() - 1; i > 0; --i)
    {
        int32 j = FMath::RandRange(0, i);
        ValidOptions.Swap(i, j);
    }

    int32 FinalCount = FMath::Min(Count, ValidOptions.Num());
    for (int32 i = 0; i < FinalCount; ++i)
    {
        Result.Add(ValidOptions[i]);
    }

    return Result;
}

TArray<FGF_ActiveItemData> UInventoryComponent::GetRandomActiveOptions(int32 Count)
{
    TArray<FGF_ActiveItemData> Result;

    if (!ActiveItemTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("액티브 할당 X"));
        return Result;
    }

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

            if (!bAlreadyOwned)
            {
                ValidOptions.Add(*RowPtr);
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
    for (int32 i = 0; i < FinalCount; ++i)
    {
        Result.Add(ValidOptions[i]);
    }

    return Result;
}

void UInventoryComponent::AddPassive(FGF_PassiveItemData NewData)
{
    OwnedPassives.Add(NewData);
    UE_LOG(LogTemp, Log, TEXT("Passive Item Added: %s"), *NewData.ItemName.ToString());
}

void UInventoryComponent::AddActive(FGF_ActiveItemData NewData)
{
    OwnedActives.Add(NewData);
    UE_LOG(LogTemp, Log, TEXT("Active Item Added: %s"), *NewData.ItemName.ToString());
}

void UInventoryComponent::AddMaterial(int32 Amount)
{
    UpgradeMaterialCount += Amount;
}
