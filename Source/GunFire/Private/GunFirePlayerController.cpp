#include "GunFirePlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

AGunFirePlayerController::AGunFirePlayerController()
    :   DefaultMappingContext(nullptr),
        JumpAction(nullptr),
        MoveAction(nullptr),
        LookAction(nullptr),
        DashAction(nullptr),
        RunAction(nullptr),
        ShotAction(nullptr),
        MeleeAttackAction(nullptr),
        InteractionAction(nullptr),
        ReloadAction(nullptr),
        SkillAction(nullptr),
        AimingAction(nullptr),
        KillTestAction(nullptr),
        LockOnAction(nullptr)
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

void AGunFirePlayerController::ShowGameOverUI()
{
    if (!GameOverWidgetClass) return;

    if (!IsValid(GameOverWidget))
    {
        GameOverWidget = CreateWidget<UUserWidget>(this, GameOverWidgetClass);
        if (!IsValid(GameOverWidget)) return;
    }

    GameOverWidget->AddToViewport();

    bShowMouseCursor = true;

    FInputModeUIOnly InputMode;
    InputMode.SetWidgetToFocus(GameOverWidget->TakeWidget());
    SetInputMode(InputMode);
}

void AGunFirePlayerController::GotoMainMenu()
{
    bShowMouseCursor = true;

    FInputModeUIOnly InputMode;
    SetInputMode(InputMode);

    UGameplayStatics::OpenLevel(this, TEXT("MainMenuLevel"));
}
