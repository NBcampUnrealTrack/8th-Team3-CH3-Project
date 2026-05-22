#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemSystemTypes.h"
#include "Engine/DataTable.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GUNFIRE_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInventoryComponent();

    // UI에서 이벤트를 바인딩할 수 있도록 BlueprintAssignable 추가
    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnInventoryChanged OnInventoryChanged;

    // ---------------------------------------------------------
    // 데이터 테이블 변수
    // ---------------------------------------------------------
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    UDataTable* PassiveItemTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    UDataTable* ActiveItemTable;

    // ---------------------------------------------------------
    // 인벤토리 메인 바구니 배열들
    // ---------------------------------------------------------
    // 장비와 재료가 모두 함께 담기는 메인 패시브 배열
    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    TArray<FGF_PassiveItemData> OwnedPassives;

    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    TArray<FGF_ActiveItemData> OwnedActives;

    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    TArray<FGF_PassiveItemData> OwnedMaterials;

    // ---------------------------------------------------------
    // 주요 함수 선언부
    // ---------------------------------------------------------
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    TArray<FGF_PassiveItemData> GetRandomPassiveOptions(int32 Count);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    TArray<FGF_ActiveItemData> GetRandomActiveOptions(int32 Count);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void AddPassive(FGF_PassiveItemData NewData);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void AddActive(FGF_ActiveItemData NewData);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void AddMaterial(FGF_PassiveItemData NewData);

    // 아이템 강화 함수
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool UpgradeItem(int32 TargetIndex);

    // 재료 소모 함수 (OwnedPassives 배열 기준)
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool ConsumeMaterial(int32 MaterialIndex);
};
