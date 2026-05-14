#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GunFirePlayerController.h"
#include "EnhancedInputComponent.h"
#include "Combat/CombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GunFire/GunFireGameMode.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

    // 스프링암 생성
    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComponent->SetupAttachment(RootComponent);
    SpringArmComponent->TargetArmLength = 300.0f;
    SpringArmComponent->bUsePawnControlRotation = true;

    // 카메라
    ThirdPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
    ThirdPersonCameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
    ThirdPersonCameraComponent->bUsePawnControlRotation = false;

    // 대쉬 설정
    bcanDash = true;
    DashStrength = 2000.0f;
    DashCooldown = 3.0f;

    // 이동속도
    NormalSpeed = 600.0f;
    RunSpeedMultiplier = 1.5f;
    RunSpeed = NormalSpeed * RunSpeedMultiplier;
    GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

    // 조준
    bIsAiming = false;
    DefaultFOV = ThirdPersonCameraComponent->FieldOfView;
    AimFOV = 70.0f;
    DefaultSocketOffset = ThirdPersonCameraComponent->GetComponentLocation();

    // 공격
    HeavyAttackHoldTime = 0.5f;
    bHeavyAttackTriggered = false;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

    //DefaultSocketOffset = ThirdPersonCameraComponent->GetComponentLocation();
    DefaultSocketOffset = FVector(0.f, 0.f, 60.f);
    AimSocketOffset = FVector(0.f, 50.f, 60.f);

    CombatComponent = FindComponentByClass<UCombatComponent>();
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    float TargetFOV = bIsAiming ? AimFOV : DefaultFOV;
    FVector TargetOffset = bIsAiming ? AimSocketOffset : DefaultSocketOffset;
    float TargetArmLength = bIsAiming ? 150.f : 300.f;

    // 부드럽게 보간 (FInterpTo 사용)
    ThirdPersonCameraComponent->FieldOfView = FMath::FInterpTo(ThirdPersonCameraComponent->FieldOfView, TargetFOV, DeltaTime, 10.f);
    SpringArmComponent->SocketOffset = FMath::VInterpTo(SpringArmComponent->SocketOffset, TargetOffset, DeltaTime, 10.f);
    SpringArmComponent->TargetArmLength = FMath::FInterpTo(SpringArmComponent->TargetArmLength, TargetArmLength, DeltaTime, 10.f);

}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    // 입력 세팅
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // 소유 중인 컨트롤러 캐스팅
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
                EnhancedInputComponent->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
            }

            // 시점
            if (PlayerController->LookAction)
            {
                EnhancedInputComponent->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
            }

            // 대쉬
            if (PlayerController->DashAction)
            {
                EnhancedInputComponent->BindAction(PlayerController->DashAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Dash);
            }

            // 달리기
            if (PlayerController->RunAction)
            {
                EnhancedInputComponent->BindAction(PlayerController->RunAction, ETriggerEvent::Started, this, &APlayerCharacter::Run);
                EnhancedInputComponent->BindAction(PlayerController->RunAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopRun);
            }

            // 사격
            if (PlayerController->ShotAction)
            {
                EnhancedInputComponent->BindAction(PlayerController->ShotAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Shot);
            }

            // 조준
            if (PlayerController->AimingAction)
            {
                //EnhancedInputComponent->BindAction(PlayerController->AimingAction, ETriggerEvent::Started, this, &APlayerCharacter::Aiming);
                //EnhancedInputComponent->BindAction(PlayerController->AimingAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopAiming);
                EnhancedInputComponent->BindAction(PlayerController->AimingAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Aiming);
            }

            // 재장전
            if (PlayerController->ReloadAction)
            {
                EnhancedInputComponent->BindAction(PlayerController->ReloadAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Reload);
            }

            // 근접 공격
            if (PlayerController->MeleeAttackAction)
            {
                EnhancedInputComponent->BindAction(PlayerController->MeleeAttackAction,
                    ETriggerEvent::Started, this, &APlayerCharacter::MeleeAttackStarted);

                EnhancedInputComponent->BindAction(PlayerController->MeleeAttackAction,
                    ETriggerEvent::Completed, this, &APlayerCharacter::MeleeAttackReleased);

                EnhancedInputComponent->BindAction(PlayerController->MeleeAttackAction,
                    ETriggerEvent::Canceled, this, &APlayerCharacter::MeleeAttackReleased);
            }

            // 스킬
            if (PlayerController->SkillAction)
            {
                EnhancedInputComponent->BindAction(PlayerController->SkillAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Skill);
            }

            // 상호작용
            if (PlayerController->InteractionAction)
            {
                EnhancedInputComponent->BindAction(PlayerController->InteractionAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Interaction);
            }


            // 디버그용 몬스터 처치 액션
            if (PlayerController->KillTestAction)
            {
                EnhancedInputComponent->BindAction(PlayerController->KillTestAction, ETriggerEvent::Started, this, &APlayerCharacter::KillEnemyForDebug);
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
    }
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
    if (!Controller) return;

    if (CombatComponent && !CombatComponent->CanMove()) return;

    FVector2D MovementVector = Value.Get<FVector2D>();

    if (MovementVector.IsNearlyZero())   return;

    // 컨트롤러의 회전값(카메라가 보는 방향)
    const FRotator Rotation = Controller->GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0); // Pitch와 Roll은 무시하고 Yaw(좌우)만 사용

    // 해당 회전 방향을 기준으로 '월드 기준 앞'과 '월드 기준 오른쪽' 방향 벡터를 계산합니다.
    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    AddMovementInput(ForwardDirection, MovementVector.X);
    AddMovementInput(RightDirection, MovementVector.Y);
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
    FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);
    }
}

void APlayerCharacter::Jump()
{
    if (CombatComponent && !CombatComponent->CanMove()) return;

    Super::Jump();
}

void APlayerCharacter::Dash(const FInputActionValue& Value)
{
    if (!Controller) return;

    if (!bcanDash)
    {
        UE_LOG(LogTemp, Log, TEXT("Dash Cooldown"));
        return;
    }

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

        // 마찰력 복구
        GetWorldTimerManager().SetTimer(DashStopTimerHandle, this, &APlayerCharacter::StopDash, 0.15f, false);

        UE_LOG(LogTemp, Log, TEXT("Dash Start"));
        bcanDash = false;

        // 쿨타임 이후에 대쉬 다시 사용가능
        GetWorldTimerManager().SetTimer(DashCooldownTimerHandle, this, &APlayerCharacter::ResetDash, DashCooldown, false);
    }
}

void APlayerCharacter::StopDash()
{
    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    if (!MoveComp) return;

    // 원래의 마찰력으로 복구
    MoveComp->GroundFriction = DefaultGroundFriction;
    MoveComp->BrakingDecelerationWalking = DefaultBrakingDeceleration;
    UE_LOG(LogTemp, Log, TEXT("Dash Stop"));
}

void APlayerCharacter::ResetDash()
{
    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    if (!MoveComp) return;
    bcanDash = true;
    UE_LOG(LogTemp, Log, TEXT("Dash On"));
}

void APlayerCharacter::Run(const FInputActionValue& Value)
{
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
    }
}

void APlayerCharacter::StopRun()
{
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
    }
}

void APlayerCharacter::Shot(const FInputActionValue& Value)
{

}

void APlayerCharacter::Aiming(const FInputActionValue& Value)
{
    if (!Controller) return;

    if (CombatComponent && !CombatComponent->CanMove()) return;

    if (!bIsAiming)
    {
        bIsAiming = true;
        GetCharacterMovement()->bOrientRotationToMovement = false; // 이동 방향 회전 끄기
        bUseControllerRotationYaw = true;
    }
    else
    {
        bIsAiming = false;
        GetCharacterMovement()->bOrientRotationToMovement = true; // 이동 방향 회전 끄기
        bUseControllerRotationYaw = false;
    }
}

void APlayerCharacter::StopAiming()
{
    if (!Controller) return;

    bIsAiming = false;
}

void APlayerCharacter::Reload(const FInputActionValue& Value)
{

}

void APlayerCharacter::MeleeAttack(const FInputActionValue& Value)
{

}

void APlayerCharacter::Skill(const FInputActionValue& Value)
{

}

void APlayerCharacter::Interaction(const FInputActionValue& Value)
{

}

void APlayerCharacter::KillEnemyForDebug()
{
    // 테스트로 현재 방의 적을 처치함
    if (AGunFireGameMode* GFGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<AGunFireGameMode>() : nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Kill 1 Enemy for Test"));
        GFGameMode->KillEnemyForTest();
    }
}

void APlayerCharacter::MeleeAttackStarted()
{
    bHeavyAttackTriggered = false;

    UE_LOG(LogTemp, Warning, TEXT("키 입력"));

    // 기존에 타이머 남아있다면 제거
    GetWorldTimerManager().ClearTimer(HeavyAttackTimerHandle);

    // 시간이 지나면 자동으로 강공격 발동하게
    GetWorldTimerManager().SetTimer(
        HeavyAttackTimerHandle,
        this,
        &APlayerCharacter::OnHeavyAttack,
        HeavyAttackHoldTime,
        false
        );
}

void APlayerCharacter::MeleeAttackReleased()
{
    bool bWatingHeavyAttack =
        GetWorldTimerManager().IsTimerActive(HeavyAttackTimerHandle);

    GetWorldTimerManager().ClearTimer(HeavyAttackTimerHandle);

    // 강공격이 이미 발동한 상태에서 키를 떼면 동작 X
    if (bHeavyAttackTriggered) return;

    if (bWatingHeavyAttack && IsValid(CombatComponent))
    {
        CombatComponent->TryLightAttack();
    }
}


void APlayerCharacter::OnHeavyAttack()
{
    bHeavyAttackTriggered = true;

    if (IsValid(CombatComponent))
    {
        CombatComponent->TryHeavyAttack();
    }
}
