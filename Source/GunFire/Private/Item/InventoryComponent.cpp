#include "Item/InventoryComponent.h"

#include "StatComponent.h"
#include "Item/ItemSystemTypes.h"
#include "Engine/DataTable.h"
#include "Game/SessionData.h"

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

            FGF_PassiveItemData* RowData = PassiveItemTable->FindRow<FGF_PassiveItemData>(OwnedItem.ItemRowName, TEXT("AddPassive_MultiStat"));
            if (RowData)
            {
                int32 Stack = OwnedItem.StackCount;

                if (OwnedItem.CurrentLevel > 0)
                {
                    OwnedItem.AttackPower_Pct = RowData->Upgrade_AttackPower_Pct * Stack;
                    OwnedItem.MaxHealth_Pct = RowData->Upgrade_MaxHealth_Pct * Stack;
                    OwnedItem.Defense_Pct = RowData->Upgrade_Defense_Pct * Stack;
                    OwnedItem.MaxStamina_Pct = RowData->Upgrade_MaxStamina_Pct * Stack;
                    OwnedItem.StaminaRegen_Pct = RowData->Upgrade_StaminaRegen_Pct * Stack;
                }
                else
                {
                    OwnedItem.AttackPower_Pct = RowData->AttackPower_Pct * Stack;
                    OwnedItem.MaxHealth_Pct = RowData->MaxHealth_Pct * Stack;
                    OwnedItem.Defense_Pct = RowData->Defense_Pct * Stack;
                    OwnedItem.MaxStamina_Pct = RowData->MaxStamina_Pct * Stack;
                    OwnedItem.StaminaRegen_Pct = RowData->StaminaRegen_Pct * Stack;
                }
            }

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

    UpdatePassiveStats();

    OnInventoryChanged.Broadcast();
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

    OnInventoryChanged.Broadcast();
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

    OnInventoryChanged.Broadcast();
}

bool UInventoryComponent::UpgradeItem(int32 TargetIndex)
{
    if (!OwnedPassives.IsValidIndex(TargetIndex)) return false;
    if (!PassiveItemTable) return false;
    if (OwnedPassives[TargetIndex].ItemRowName.IsNone()) return false;

    FGF_PassiveItemData* RowData = PassiveItemTable->FindRow<FGF_PassiveItemData>(OwnedPassives[TargetIndex].ItemRowName, TEXT("UpgradeItem"));
    if (!RowData) return false;

    OwnedPassives[TargetIndex].ItemName = RowData->UpgradeItemName;
    OwnedPassives[TargetIndex].ItemDescription = RowData->UpgradeItemDescription;
    OwnedPassives[TargetIndex].ItemIcon = RowData->ItemIcon;

    OwnedPassives[TargetIndex].CurrentLevel += 1;

    // 업그레이드 시 덮어쓰기
    int32 Stack = OwnedPassives[TargetIndex].StackCount;
    OwnedPassives[TargetIndex].AttackPower_Pct = RowData->Upgrade_AttackPower_Pct * Stack;
    OwnedPassives[TargetIndex].MaxHealth_Pct = RowData->Upgrade_MaxHealth_Pct * Stack;
    OwnedPassives[TargetIndex].Defense_Pct = RowData->Upgrade_Defense_Pct * Stack;
    OwnedPassives[TargetIndex].MaxStamina_Pct = RowData->Upgrade_MaxStamina_Pct * Stack;
    OwnedPassives[TargetIndex].StaminaRegen_Pct = RowData->Upgrade_StaminaRegen_Pct * Stack;

    UpdatePassiveStats();

    OnInventoryChanged.Broadcast();
    return true;
}

bool UInventoryComponent::ConsumeMaterial(int32 DummyIndex)
{
    int32 FoundMaterialIndex = INDEX_NONE;

    for (int32 i = 0; i < OwnedPassives.Num(); ++i)
    {
        if (OwnedPassives[i].ItemType == EGF_ItemType::Material)
        {
            FoundMaterialIndex = i;
            break;
        }
    }

    if (FoundMaterialIndex == INDEX_NONE) return false;
    if (OwnedPassives[FoundMaterialIndex].StackCount <= 0) return false;

    OwnedPassives[FoundMaterialIndex].StackCount = FMath::Max(0, OwnedPassives[FoundMaterialIndex].StackCount - 1);

    OnInventoryChanged.Broadcast();
    return true;
}

void UInventoryComponent::SetInventorySessionData(const FInventorySessionData& InventorySessionData)
{
    OwnedPassives = InventorySessionData.OwnedPassives;
    OwnedActives = InventorySessionData.OwnedActives;
    OwnedMaterials = InventorySessionData.OwnedMaterials;

    UpdatePassiveStats();

    OnInventoryChanged.Broadcast();
}

void UInventoryComponent::UpdatePassiveStats()
{
    AActor* Owner = GetOwner();
    if (!IsValid(Owner)) return;

    UStatComponent* StatComp = Owner->FindComponentByClass<UStatComponent>();
    if (!IsValid(StatComp)) return;

    StatComp->ResetModifier();

    for (const FGF_PassiveItemData& Item : OwnedPassives)
    {
        // 아이템타입이 패시브가 아닌 경우 건너뛰기
        if (Item.ItemType != EGF_ItemType::Passive) continue;

        FName SourceID = Item.ItemRowName.IsNone()
            ? FName(*Item.ItemName.ToString())
            : Item.ItemRowName;

        AddPassiveModifier(StatComp, SourceID, ECombatStatType::AttackPower, Item.AttackPower_Pct);
        AddPassiveModifier(StatComp, SourceID, ECombatStatType::MaxHealth, Item.MaxHealth_Pct);
        AddPassiveModifier(StatComp, SourceID, ECombatStatType::Defense, Item.Defense_Pct);
        AddPassiveModifier(StatComp, SourceID, ECombatStatType::MaxStamina, Item.MaxStamina_Pct);
        AddPassiveModifier(StatComp, SourceID, ECombatStatType::StaminaRegen, Item.StaminaRegen_Pct);

        UE_LOG(LogTemp, Warning, TEXT("TEST : %f"), Item.AttackPower_Pct);
    }
}

void UInventoryComponent::AddPassiveModifier(UStatComponent* StatComponent, FName SourceID, ECombatStatType StatType,
    float Value)
{
    if (!IsValid(StatComponent)) return;
    if (FMath::IsNearlyZero(Value)) return;

    FStatModifier Modifier;
    Modifier.SourceID = SourceID;
    Modifier.StatType = StatType;
    Modifier.ModifierType = EStatModifierType::Multiply;
    Modifier.Value = Value;

    StatComponent->AddModifier(Modifier);
}
