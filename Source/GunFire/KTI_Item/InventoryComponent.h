#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemSystemTypes.h" 
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChangedSignature);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GUNFIRE_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInventoryComponent();

    UPROPERTY(BlueprintAssignable, Category = "Inventory|Event")
    FOnInventoryChangedSignature OnInventoryChanged;

    // 랜덤 목록 가져오기
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    TArray<FGF_PassiveItemData> GetRandomPassiveOptions(int32 Count);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    TArray<FGF_ActiveItemData> GetRandomActiveOptions(int32 Count);

    // 아이템 추가 함수 
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void AddPassive(FGF_PassiveItemData NewData);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void AddActive(FGF_ActiveItemData NewData);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void AddMaterial(FGF_PassiveItemData NewData);

    UFUNCTION(BlueprintCallable, Category = "Inventory|Upgrade")
    int32 UpgradeItem(int32 TargetIndex, int32 MaterialIndex);

    // 보유 리스트 반환
    UFUNCTION(BlueprintPure, Category = "Inventory")
    TArray<FGF_PassiveItemData> GetOwnedPassives() const { return OwnedPassives; }

    UFUNCTION(BlueprintPure, Category = "Inventory")
    TArray<FGF_ActiveItemData> GetOwnedActives() const { return OwnedActives; }

    UFUNCTION(BlueprintPure, Category = "Inventory")
    TArray<FGF_PassiveItemData> GetOwnedMaterials() const { return OwnedMaterials; }

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    class UDataTable* PassiveItemTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    class UDataTable* ActiveItemTable;

    // 보유 데이터 배열
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TArray<FGF_PassiveItemData> OwnedPassives;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TArray<FGF_ActiveItemData> OwnedActives;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TArray<FGF_PassiveItemData> OwnedMaterials;
};
