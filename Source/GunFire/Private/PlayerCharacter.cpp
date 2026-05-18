#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GunFirePlayerController.h"
#include "EnhancedInputComponent.h"
#include "Combat/CombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GunFire/GunFireGameMode.h"
#include "Interactables/InteractableInterface.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "CollisionQueryParams.h"
#include "StatComponent.h"
#include "WorldCollision.h"
#include "GameFramework/Actor.h"

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
    CanDash = true;
    DashStrength = 2000.0f;
    DashCooldown = 3.0f;

    // 이동속도
    NormalSpeed = 600.0f;
    RunSpeedMultiplier = 1.5f;
    RunSpeed = NormalSpeed * RunSpeedMultiplier;
    GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

    // 조준
    IsAiming = false;
    DefaultFOV = ThirdPersonCameraComponent->FieldOfView;
    AimFOV = 70.0f;
    DefaultSocketOffset = ThirdPersonCameraComponent->GetComponentLocation();

    // 사격
    MaxAmmo = 4;
    TotalAmmo = 12;
    CurrentAmmo = MaxAmmo;
    AmmoPerFire = 1;
    CanFire = true;
    Rof = 1.0f;

    // 재장전
    IsReloading = false;
    ReloadTime = 2.0f;

    // 공격
    HeavyAttackHoldTime = 0.5f;
    bHeavyAttackTriggered = false;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

    DefaultSocketOffset = FVector(0.f, 0.f, 0.f);
    AimSocketOffset = FVector(0.f, 50.f, 0.f);

    // 상호작용 범위 감지 0.2마다 실행
    GetWorldTimerManager().SetTimer(InteractionCheckTimerHandle, this, &APlayerCharacter::CheckInteractablesRamge, 0.2f, true);
    // // 디버그용 스태미너 회복
    // GetWorldTimerManager().SetTimer(NaturalHealingStaminaTimerHandle, this, &APlayerCharacter::NaturalHealingStamina, 1.0f, true);

}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    //CheckForInteractables();

    // 시야각
    float TargetFOV = IsAiming ? AimFOV : DefaultFOV;
    // 카메라 위치
    FVector TargetOffset = IsAiming ? AimSocketOffset : DefaultSocketOffset;
    // 스프링암 길이
    float TargetArmLength = IsAiming ? 450.f : 500.f;

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
                EnhancedInputComponent->BindAction(PlayerController->RunAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Run);
            }

            // 사격
            if (PlayerController->ShotAction)
            {
                EnhancedInputComponent->BindAction(PlayerController->ShotAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Shot);
            }

            // 조준
            if (PlayerController->AimingAction)
            {
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

void APlayerCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    CombatComponent = FindComponentByClass<UCombatComponent>();
    StatComponent = FindComponentByClass<UStatComponent>();

    // 스탯 컴포넌트의 델리게이트 이벤트 바인딩
    if (IsValid(StatComponent))
    {
        StatComponent->OnDamaged.AddDynamic(this, &APlayerCharacter::HandleDamaged);
        StatComponent->OnHealed.AddDynamic(this, &APlayerCharacter::HandleHealed);
        StatComponent->OnDead.AddDynamic(this, &APlayerCharacter::HandleDead);
        StatComponent->OnHealthChanged.AddDynamic(this, &APlayerCharacter::HandleHealthChanged);
        StatComponent->OnStaminaChanged.AddDynamic(this, &APlayerCharacter::HandleStaminaChanged);
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
    if (!Controller) return;

    FVector2D LookAxisVector = Value.Get<FVector2D>();

    AddControllerYawInput(LookAxisVector.X);
    AddControllerPitchInput(LookAxisVector.Y);
}

void APlayerCharacter::Jump()
{
    if (CombatComponent && !CombatComponent->CanMove()) return;

    Super::Jump();
}

void APlayerCharacter::Dash(const FInputActionValue& Value)
{
    if (!Controller) return;

    if (!CanDash)
    {
        UE_LOG(LogTemp, Log, TEXT("Dash Cooldown"));
        return;
    }

    UCharacterMovementComponent* MoveComp = GetCharacterMovement();

    // 공중 상태 확인
    if (MoveComp && MoveComp->IsFalling())
    {
        //공중이면 함수 종료
        return;
    }

    //if (CurrentStamina < 20) return;

    //CurrentStamina = FMath::Clamp(CurrentStamina - 20, 0, MaxStamina);
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
    CanDash = false;

    // 쿨타임 이후에 대쉬 다시 사용가능
    GetWorldTimerManager().SetTimer(DashCooldownTimerHandle, this, &APlayerCharacter::ResetDash, DashCooldown, false);
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
    CanDash = true;
    UE_LOG(LogTemp, Log, TEXT("Dash On"));
}

void APlayerCharacter::Run(const FInputActionValue& Value)
{
    if (GetCharacterMovement())
    {
        if (Value.Get<bool>())
        {
            //if (CurrentStamina <= 0) return;
            GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
            //CurrentStamina = FMath::Clamp(CurrentStamina - 1, 0, MaxStamina);
        }
        else
        {
            GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
        }
    }
}

void APlayerCharacter::StopRun()
{
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
    }
}

void APlayerCharacter::Aiming(const FInputActionValue& Value)
{
    if (!Controller) return;

    if (CombatComponent && !CombatComponent->CanMove()) return;

    if (!IsAiming)
    {
        IsAiming = true;
        GetCharacterMovement()->bOrientRotationToMovement = false; // 이동 방향 회전 끄기
        bUseControllerRotationYaw = true;
    }
    else
    {
        IsAiming = false;
        GetCharacterMovement()->bOrientRotationToMovement = true; // 이동 방향 회전
        bUseControllerRotationYaw = false;
    }
}

void APlayerCharacter::StopAiming()
{
    if (!Controller) return;

    IsAiming = false;
}

void APlayerCharacter::Shot(const FInputActionValue& Value)
{
    if (!Controller) return;

    if (CombatComponent && !CombatComponent->CanMove()) return;

    if (!IsAiming || !CanFire || IsReloading) return;

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

    if (CurrentAmmo > 0 && AnimInstance && !AnimInstance->Montage_IsPlaying(FireMontage) && !AnimInstance->Montage_IsPlaying(FireDelayMontage))
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Fire"));

        CurrentAmmo -= AmmoPerFire;
        CanFire = false;

        AnimInstance->Montage_Play(FireMontage);

        GetWorldTimerManager().SetTimer(ShotDelayTimerHandle, this, &APlayerCharacter::ShotDelay, 0.5f, false);
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Ammo Empty OR Delay"));
    }
}

void APlayerCharacter::ShotDelay()
{
    GetWorld()->GetTimerManager().ClearTimer(ShotDelayTimerHandle);
    CanFire = true;

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && FireDelayMontage)
    {
        AnimInstance->Montage_Play(FireDelayMontage);
    }
}

void APlayerCharacter::Reload(const FInputActionValue& Value)
{
    if (!Controller) return;

    if (CurrentAmmo == MaxAmmo || !CanFire || IsReloading || TotalAmmo <= 0) return;

    IsReloading = true;
    CanFire = false;
    GetWorldTimerManager().SetTimer(ReloadTimerHandle, this, &APlayerCharacter::Reloading, ReloadTime, false);
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Reload"));

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && ReloadMontage)
    {
        AnimInstance->Montage_Play(ReloadMontage);
    }
}

void APlayerCharacter::Reloading()
{
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Reloading"));
    TotalAmmo = FMath::Clamp(TotalAmmo - (MaxAmmo - CurrentAmmo), 0, 30);

    if (TotalAmmo < MaxAmmo)
    {
        CurrentAmmo = TotalAmmo;
    }
    else
    {
        CurrentAmmo = MaxAmmo;
    }
    CanFire = true;
    IsReloading = false;
}

void APlayerCharacter::MeleeAttack(const FInputActionValue& Value)
{

}

void APlayerCharacter::Skill(const FInputActionValue& Value)
{

}

void APlayerCharacter::Interaction(const FInputActionValue& Value)
{
    if (TargetedActor)
    {
        IInteractableInterface::Execute_Interact(TargetedActor, this);

        TargetedActor = nullptr;
    }
}

void APlayerCharacter::CheckForInteractables()
{
    // 캐릭터의 눈 위치 또는 카메라 위치 반환
    FVector Start = GetPawnViewLocation();
    FVector End = Start + (GetViewRotation().Vector() * 300.0f);

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    // 트레이스 확인
    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
    {
        AActor* HitActor = HitResult.GetActor();

        // 트레이스에 맞은 액터가 UInteractableInterface를 가지고 있는지 확인
        if (HitActor && HitActor->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
        {
            if (TargetedActor != HitActor)
            {
                if (TargetedActor)
                {
                    // 해당 액터를 보고있지 않으면 UI비활성화
                    IInteractableInterface::Execute_LookAway(TargetedActor);
                }

                TargetedActor = HitActor;
                // 해당 액터를 보고있으면 UI 활성화
                IInteractableInterface::Execute_LookAt(TargetedActor);
                GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("InteractActor"));
            }
            return;
        }
    }

    if (TargetedActor != nullptr)
    {
        IInteractableInterface::Execute_LookAway(TargetedActor);
        TargetedActor = nullptr;
    }
}

void APlayerCharacter::CheckInteractablesRamge()
{
    FVector Center = GetActorLocation();
    float Radius = 200.0f;

    // 범위 안에 오버랩된 액터 저장
    TArray<FOverlapResult> OverlapResults;
    FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius);
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    // 범위 확인용 나중에 삭제
    DrawDebugSphere(
        GetWorld(),
        Center,          // 구체 중심
        Radius,          // 반경
        12,              // 세그먼트 (숫자가 높을수록 부드러운 구체가 됨)
        FColor::Green,   // 선 색상
        false,           // 매 프레임 새로 그릴 것이므로 false (Persistent)
        -1.f,            // 지속 시간 (-1은 이번 프레임만)
        0,               // Depth Priority
        1.f              // 선 두께
    );

    bool bHit = GetWorld()->OverlapMultiByChannel(
        OverlapResults,
        Center,
        FQuat::Identity,
        ECC_Visibility,
        Sphere,
        Params
    );

    AActor* ClosestActor = nullptr;
    float MinDistance = Radius;

    if (bHit)
    {
        for (auto& Result : OverlapResults)
        {
            // 상호작용 가능한 액터인지 확인
            AActor* PotentialTarget = Result.GetActor();
            if (PotentialTarget && PotentialTarget->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
            {
                float Distance = FVector::Dist(Center, PotentialTarget->GetActorLocation());
                if (Distance < MinDistance)
                {
                    // 시선과 상관없이 '거리'만으로 판단
                    MinDistance = Distance;
                    // 가장 가까운 아이템
                    ClosestActor = PotentialTarget;
                }
            }
        }
    }

    // UI 온/오프
    if (ClosestActor != TargetedActor)
    {
        if (TargetedActor) IInteractableInterface::Execute_LookAway(TargetedActor);
        TargetedActor = ClosestActor;
        if (TargetedActor) IInteractableInterface::Execute_LookAt(TargetedActor);
    }
}

void APlayerCharacter::HandleDamaged(float ActualDamage, AController* DamagedInstigator)
{
    // C++ 코드 처리
}

void APlayerCharacter::HandleHealed(float HealAmount)
{
    // C++ 코드 처리
}

void APlayerCharacter::HandleDead(AController* DamagedInstigator)
{
    // C++ 코드 처리
}

void APlayerCharacter::HandleHealthChanged(float CurrentHealth, float MaxHealth)
{
    // C++ 코드 처리
}

void APlayerCharacter::HandleStaminaChanged(float CurrentStamina, float MaxStamina)
{
    // C++ 코드 처리
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
