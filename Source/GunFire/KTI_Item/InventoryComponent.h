#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemSystemTypes.h"
#include "InventoryComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GUNFIRE_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInventoryComponent();

    // 랜덤 패시브 목록 가져오기
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    TArray<FGF_PassiveItemData> GetRandomPassiveOptions(int32 Count);

    // 랜덤 액티브 목록 가져오기
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    TArray<FGF_ActiveItemData> GetRandomActiveOptions(int32 Count);

    // 패시브 추가
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void AddPassive(FGF_PassiveItemData NewData);

    // 액티브 추가
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void AddActive(FGF_ActiveItemData NewData);

    // 재료 추가
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void AddMaterial(int32 Amount);

    // 보유 패시브 리스트 반환 
    UFUNCTION(BlueprintPure, Category = "Inventory")
    TArray<FGF_PassiveItemData> GetOwnedPassives() const { return OwnedPassives; }

    // 보유 액티브 리스트 반환
    UFUNCTION(BlueprintPure, Category = "Inventory")
    TArray<FGF_ActiveItemData> GetOwnedActives() const { return OwnedActives; }

    // 재료 개수 반환
    UFUNCTION(BlueprintPure, Category = "Inventory")
    int32 GetMaterialCount() const { return UpgradeMaterialCount; }

protected:
    // 데이터 테이블
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    class UDataTable* PassiveItemTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    class UDataTable* ActiveItemTable;

    // 보유 리스트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TArray<FGF_PassiveItemData> OwnedPassives;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TArray<FGF_ActiveItemData> OwnedActives;

    // 재료 개수
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    int32 UpgradeMaterialCount;
};
