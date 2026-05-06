#include "GunFirePlayerController.h"
#include "EnhancedInputSubsystems.h"

AGunFirePlayerController::AGunFirePlayerController()
    :   DefaultMappingContext(nullptr),
        JumpAction(nullptr),
        MoveAction(nullptr),
        LookAction(nullptr),
        DashAction(nullptr)
{
}

void AGunFirePlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (DefaultMappingContext)
            {
                Subsystem->AddMappingContext(DefaultMappingContext, 0);
            }
        }
    }
}
