#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "InventoryHUD.generated.h"

UCLASS()
class GUNFIRE_API AInventoryHUD : public AHUD
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "Inventory")
    TSubclassOf<class UUserWidget> InventoryWidgetClass;

    UPROPERTY()
    class UUserWidget* InventoryWidgetInstance;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void ToggleInventory();

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void ShowInventory();

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void HideInventory();
};
