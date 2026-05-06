#include "GunFireCharacter.h"
#include "GunFireProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "GunFirePlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AGunFireCharacter::AGunFireCharacter()
{
	// 콜리전 컴포넌트 사이즈
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// 카메라 컴포넌트 생성
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // 카메라 위치
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// 플레이어 캐릭터 메시 생성
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

    // 대쉬 설정
    bcanDash = true;
    DashStrength = 2000.0f;
    DashCooldown = 3.0f;
}

//////////////////////////////////////////////////////////////////////////// 입력

void AGunFireCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// 입력 매핑 추가
	/*if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}*/
}

void AGunFireCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 입력 세팅
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
        if (AGunFirePlayerController* PlayerController = Cast<AGunFirePlayerController>(GetController()))
        {
            // 점프
            if (PlayerController->JumpAction)
            {
                EnhancedInputComponent->BindAction(PlayerController->JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
                EnhancedInputComponent->BindAction(PlayerController->JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
            }

            // 이동
            if (PlayerController->MoveAction)
            {
                EnhancedInputComponent->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this, &AGunFireCharacter::Move);
            }

            // 시점
            if (PlayerController->LookAction)
            {
                EnhancedInputComponent->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered, this, &AGunFireCharacter::Look);
            }

            // 대쉬
            if (PlayerController->DashAction)
            {
                EnhancedInputComponent->BindAction(PlayerController->DashAction, ETriggerEvent::Triggered, this, &AGunFireCharacter::Dash);
            }
        }
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void AGunFireCharacter::Move(const FInputActionValue& Value)
{
    if (!Controller) return;

	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddMovementInput(GetActorForwardVector(), MovementVector.X);
		AddMovementInput(GetActorRightVector(), MovementVector.Y);
	}
}

void AGunFireCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AGunFireCharacter::Dash(const FInputActionValue& Value)
{
    if (!Controller) return;

    if (bcanDash)
    {
        UCharacterMovementComponent* MoveComp = GetCharacterMovement();

        // 대쉬 방향 결정 (입력 방향 위주)
        FVector DashDirection = MoveComp->GetLastInputVector();

        // 만약 이동 입력이 없다면(가만히 있을 때) 캐릭터의 전방으로 대쉬
        if (DashDirection.IsNearlyZero())
        {
            DashDirection = GetActorForwardVector();
        }

        // 현재의 마찰력 설정값 저장
        DefaultGroundFriction = MoveComp->GroundFriction;
        DefaultBrakingDeceleration = MoveComp->BrakingDecelerationWalking;

        // 마찰력과 제동력을 0으로 설정
        MoveComp->GroundFriction = 0.f;
        MoveComp->BrakingDecelerationWalking = 0.f;

        // 즉각적인 속도 부여
        MoveComp->Velocity = DashDirection.GetSafeNormal() * DashStrength;

        GetWorldTimerManager().SetTimer(DashStopTimerHandle, this, &AGunFireCharacter::StopDash, 0.15f, false);

        UE_LOG(LogTemp, Log, TEXT("Dash Cooldown"));
        bcanDash = false;
        GetWorldTimerManager().SetTimer(DashCooldownTimerHandle, this, &AGunFireCharacter::ResetDash, DashCooldown, false);
    }
}

void AGunFireCharacter::ResetDash()
{
    bcanDash = true;
    UE_LOG(LogTemp, Log, TEXT("Dash On"));
}

void AGunFireCharacter::StopDash()
{
    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    if (!MoveComp) return;

    // 원래의 마찰력으로 복구
    MoveComp->GroundFriction = DefaultGroundFriction;
    MoveComp->BrakingDecelerationWalking = DefaultBrakingDeceleration;
}
