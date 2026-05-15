#include "Interactables/InteractTestItem.h"
#include "Components/WidgetComponent.h"

AInteractTestItem::AInteractTestItem()
{
	PrimaryActorTick.bCanEverTick = false;

    InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
    InteractionWidget->SetupAttachment(RootComponent);

    InteractionWidget->SetVisibility(false);
    InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
}

void AInteractTestItem::Interact_Implementation(AActor* Interactor)
{
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("아이템 상호작용"));
    Destroy();
}

void AInteractTestItem::LookAt_Implementation()
{
    if (InteractionWidget)
    {
        InteractionWidget->SetVisibility(true);
    }
}

void AInteractTestItem::LookAway_Implementation()
{
    if (InteractionWidget)
    {
        InteractionWidget->SetVisibility(false);
    }
}

