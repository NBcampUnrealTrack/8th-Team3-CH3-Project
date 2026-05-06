#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemSystemTypes.h" 
#include "InventoryComponent.generated.h"

class UDataTable;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GUNFIRE_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInventoryComponent();


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    TObjectPtr<UDataTable> PassiveItemTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    TObjectPtr<UDataTable> ActiveItemTable;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 UpgradeMaterialCount;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TArray<FGF_PassiveItemData> OwnedPassives;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TArray<FGF_ActiveItemData> OwnedActives;


    UFUNCTION(BlueprintCallable, Category = "Inventory")
    TArray<FGF_PassiveItemData> GetRandomPassiveOptions(int32 Count = 3);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    TArray<FGF_ActiveItemData> GetRandomActiveOptions(int32 Count = 3);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void AddPassive(FGF_PassiveItemData NewData);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void AddActive(FGF_ActiveItemData NewData);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void AddMaterial(int32 Amount);
};
