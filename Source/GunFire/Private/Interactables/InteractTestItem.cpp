#include "Interactables/InteractTestItem.h"
#include "Components/WidgetComponent.h"

AInteractTestItem::AInteractTestItem()
{
	PrimaryActorTick.bCanEverTick = false;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    SetRootComponent(Mesh);

    InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
    InteractionWidget->SetupAttachment(RootComponent);

    InteractionWidget->SetVisibility(false);
    InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
}

void AInteractTestItem::Interact_Implementation(AActor* Interactor)
{
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

