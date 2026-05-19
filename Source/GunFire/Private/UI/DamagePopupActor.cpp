#include "UI/DamagePopupActor.h"

#include "Components/WidgetComponent.h"

ADamagePopupActor::ADamagePopupActor()
{
 	PrimaryActorTick.bCanEverTick = false;

    Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
    SetRootComponent(Scene);

    WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
    WidgetComponent->SetupAttachment(Scene);
    WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
}

void ADamagePopupActor::InitDamagePopupActor(float NewDamage)
{
    OnDamagePopupInitialized(NewDamage);
}


