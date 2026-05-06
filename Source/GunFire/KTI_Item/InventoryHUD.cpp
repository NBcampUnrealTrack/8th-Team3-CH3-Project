#include "InventoryHUD.h"
#include "Blueprint/UserWidget.h"

void AInventoryHUD::ToggleInventory()
{
    if (InventoryWidgetInstance && InventoryWidgetInstance->IsInViewport())
    {
        HideInventory();
    }
    else
    {
        ShowInventory();
    }
}

void AInventoryHUD::ShowInventory()
{
    if (!InventoryWidgetInstance && InventoryWidgetClass)
    {
        InventoryWidgetInstance = CreateWidget<UUserWidget>(GetOwningPlayerController(), InventoryWidgetClass);
    }

    if (InventoryWidgetInstance)
    {
        InventoryWidgetInstance->AddToViewport();
        GetOwningPlayerController()->SetShowMouseCursor(true);
        GetOwningPlayerController()->SetInputMode(FInputModeGameAndUI());
    }
}

void AInventoryHUD::HideInventory()
{
    if (InventoryWidgetInstance)
    {
        InventoryWidgetInstance->RemoveFromParent();
        GetOwningPlayerController()->SetShowMouseCursor(false);
        GetOwningPlayerController()->SetInputMode(FInputModeGameOnly());
    }
}
