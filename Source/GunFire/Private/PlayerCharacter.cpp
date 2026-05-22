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
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "Weapon/WeaponComponent.h"
#include "Kismet/GamePlayStatics.h"
#include "Particles/ParticleSystemComponent.h"   // 파티클 제어용
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

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

    // 이동 입력
    CurrentMovementInput = FVector2D::ZeroVector;

    // 록온
    IsLockOn = false;

    // 공격
    HeavyAttackHoldTime = 0.5f;
    bHeavyAttackTriggered = false;

    // 사망
    DeadMontage = nullptr;
    HitMontage = nullptr;
}

bool APlayerCharacter::HasMovementInput() const
{
    return !CurrentMovementInput.IsNearlyZero();
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

FRotator APlayerCharacter::GetAttackInputRotation() const
{
    // 이동 입력이 없다면 방향 전환 X
    if (!HasMovementInput())
    {
        return GetActorRotation();
    }

    // 컨트롤러의 회전값을 가져옴, 3인칭 게임에선 카메라가 바라보는 방향
    FRotator ControlRotation = Controller
        ? Controller->GetControlRotation()
        : GetActorRotation();

    // 지면 기준 좌우회전만 하면 되므로 Yaw값만 남김
    FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

    // 회전에서 forward, right 추출
    // Move에서 forward에 X, Right에 Y 사용하므로 그대로 따라감
    FVector CameraForward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    FVector CameraRight = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    // 공격 시 입력에 따라 바라봐야 할 방향 벡터를 구함
    FVector DesiredDirection =
        CameraForward * CurrentMovementInput.X +
        CameraRight * CurrentMovementInput.Y;

    // 위로는 움직일 필요 없으므로 Z값 날림
    DesiredDirection.Z = 0.f;

    if (DesiredDirection.IsNearlyZero())
    {
        return GetActorRotation();
    }

    return DesiredDirection.Rotation();
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
                EnhancedInputComponent->BindAction(PlayerController->MoveAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopMoveInput);
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
                EnhancedInputComponent->BindAction(PlayerController->InteractionAction, ETriggerEvent::Started, this, &APlayerCharacter::Interaction);
            }


            // 디버그용 몬스터 처치 액션
            if (PlayerController->KillTestAction)
            {
                EnhancedInputComponent->BindAction(PlayerController->KillTestAction, ETriggerEvent::Started, this, &APlayerCharacter::KillEnemyForDebug);
            }

            // 록온
            if (PlayerController->LockOnAction)
            {
                EnhancedInputComponent->BindAction(PlayerController->LockOnAction, ETriggerEvent::Started, this, &APlayerCharacter::LockOn);
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
    }
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
    CurrentMovementInput = Value.Get<FVector2D>();
    if (CurrentMovementInput.IsNearlyZero())
    {
        CurrentMovementInput = FVector2D::ZeroVector;
        return;
    }

    if (!Controller) return;

    // 공격중과 같이 움직일 수 없는 상태라면 return
    if (CombatComponent && !CombatComponent->CanMove()) return;


    // 컨트롤러의 회전값(카메라가 보는 방향)
    const FRotator Rotation = Controller->GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0); // Pitch와 Roll은 무시하고 Yaw(좌우)만 사용

    // 해당 회전 방향을 기준으로 '월드 기준 앞'과 '월드 기준 오른쪽' 방향 벡터를 계산합니다.
    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    AddMovementInput(ForwardDirection, CurrentMovementInput.X);
    AddMovementInput(RightDirection, CurrentMovementInput.Y);
}

void APlayerCharacter::StopMoveInput()
{
    CurrentMovementInput = FVector2D::ZeroVector;
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
    if (!IsValid(CombatComponent)) return;

    CombatComponent->TryDodge();
}

bool APlayerCharacter::CanStartDash()
{
    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    return IsValid(MoveComp) && !MoveComp->IsFalling();
}

bool APlayerCharacter::StartDash(float DashStrength)
{
    if (!Controller || !DashMontage) return false;

    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    if (!IsValid(MoveComp)) return false;

    // 공중 상태면 함수 종료
    if (MoveComp->IsFalling()) return false;

    GetMesh()->SetHiddenInGame(true);

    if (UWeaponComponent* WeaponComp = FindComponentByClass<UWeaponComponent>())
    {
        WeaponComp->SetHiddenAllWeapons(true);
    }

    if (DashParticle)
    {
        // Root 본에 붙여서 캐릭터 이동 경로를 그대로 따라오게 만듭니다.
        ActiveDashParticleComp = UGameplayStatics::SpawnEmitterAttached(
            DashParticle,
            GetMesh(),
            FName(),
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            EAttachLocation::KeepRelativeOffset
        );
    }

    //if (DashNiagara)
    //{
    //    // 주로 캐릭터 발바닥(Root)이나 특정 본(Bone) 이름에 부착합니다.
    //    ActiveDashNiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
    //        DashNiagara,
    //        GetMesh(),                 // 부착할 대상 컴포넌트 (캐릭터 메쉬)
    //        FName(),
    //        FVector::ZeroVector,       // 상대 위치 오프셋
    //        FRotator::ZeroRotator,     // 상대 회전 오프셋
    //        EAttachLocation::KeepRelativeOffset,
    //        true
    //    );
    //}

    // 이동방향으로 회전 못하게 잠시 막아둠
    // 카메라 방향으로 회전 못하게 막아둠
    // 회전값 컨트롤러 동기화 못하게 막아둠
    bPrevOrientRotationToMovement = MoveComp->bOrientRotationToMovement;
    bPrevUseControllerRotationYaw = bUseControllerRotationYaw;
    bPrevUseControllerDesiredRotation = MoveComp->bUseControllerDesiredRotation;

    MoveComp->bOrientRotationToMovement = false;
    bUseControllerRotationYaw = false;
    MoveComp->bUseControllerDesiredRotation = false;

    // 대쉬 방향 결정 (입력 방향 위주)
    FVector DashDirection = GetAttackInputRotation().Vector();

    // 만약 이동 입력이 없다면(가만히 있을 때) 캐릭터의 전방으로 대쉬
    if (DashDirection.IsNearlyZero())
    {
        DashDirection = GetActorForwardVector();
    }

    // 벡터 자체의 크기(길이)가 정확히 1.0으로 고정
    DashDirection.Z = 0.f;
    DashDirection.Normalize();

    // 현재의 마찰력 설정값 저장
    DefaultGroundFriction = MoveComp->GroundFriction;
    DefaultBrakingDeceleration = MoveComp->BrakingDecelerationWalking;

    // 마찰력과 제동력을 0으로 설정
    MoveComp->GroundFriction = 0.f;
    MoveComp->BrakingDecelerationWalking = 0.f;
    MoveComp->MaxWalkSpeed = DashStrength;

    // 즉각적인 속도 부여
    MoveComp->Velocity = DashDirection * DashStrength;

    // 대쉬 몽타주 재생
    float Duration = PlayAnimMontage(DashMontage);
    if (Duration <= 0.f)
    {
        FinishDash();
        return false;
    }

    UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
    if (!AnimInstance)
    {
        FinishDash();
        return false;
    }

    // 애니메이션 몽타주가 끝날 때 호출할 함수 바인딩
    FOnMontageEnded OnDashMontageEnded;
    OnDashMontageEnded.BindUObject(this, &APlayerCharacter::HandleDashMontageEnded);
    AnimInstance->Montage_SetEndDelegate(OnDashMontageEnded, DashMontage);


    return true;
}

void APlayerCharacter::FinishDash()
{
    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    if (!MoveComp) return;

    GetMesh()->SetHiddenInGame(false);
    if (UWeaponComponent* WeaponComp = FindComponentByClass<UWeaponComponent>())
    {
        WeaponComp->SetHiddenAllWeapons(false);
    }

    MoveComp->MaxWalkSpeed = NormalSpeed;
    // 원래의 마찰력으로 복구
    MoveComp->GroundFriction = DefaultGroundFriction;
    MoveComp->BrakingDecelerationWalking = DefaultBrakingDeceleration;

    // 회전 상태 복구
    MoveComp->bOrientRotationToMovement = bPrevOrientRotationToMovement;
    bUseControllerRotationYaw = bPrevUseControllerRotationYaw;
    MoveComp->bUseControllerDesiredRotation = bPrevUseControllerDesiredRotation;

    if (ActiveDashParticleComp)
    {
        // Deactivate()를 호출하면 새로 뿜어져 나오는 파티클은 멈추고,
        // 이미 월드에 생성되어 날아가던 잔상 파티클들은 자연스럽게 수명이 다해 사라집니다.
        ActiveDashParticleComp->Deactivate();
        ActiveDashParticleComp = nullptr;
    }

    /*if (ActiveDashNiagaraComp)
    {
        ActiveDashNiagaraComp->Deactivate();
        ActiveDashNiagaraComp = nullptr;
    }*/
    // 무적 혹시 남아있다면 풀어주기
    if (IsValid(StatComponent))
    {
        StatComponent->SetInvincible(false);
    }
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

    UWeaponComponent* WeaponComp = FindComponentByClass<UWeaponComponent>();
    if (!IsValid(WeaponComp) || !WeaponComp->HasGun()) return;

    if (!IsAiming)
    {
        IsAiming = true;

        if (IsLockOn) return;

        GetCharacterMovement()->bOrientRotationToMovement = false; // 이동 방향 회전 끄기
        bUseControllerRotationYaw = true;
    }
    else
    {
        IsAiming = false;

        if (IsLockOn) return;

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
    if (!IsAiming) return;
    if (!IsValid(CombatComponent)) return;

    CombatComponent->TryRangedAttack();
}

void APlayerCharacter::Reload(const FInputActionValue& Value)
{
    if (!Controller) return;
    if (!IsValid(CombatComponent)) return;

    CombatComponent->TryReload();
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
    if (!IsValid(StatComponent)) return;

    // 이미 죽어있다면 처리 X
    if (StatComponent->IsDead()) return;

    // 피격 몽타주 재생
    if (HitMontage)
    {
        float HitStunnedTime = PlayAnimMontage(HitMontage);
        if (HitStunnedTime <= 0.f) return;

        // 전투 상태를 피격 상태로 전환
        if (IsValid(CombatComponent))
        {
            CombatComponent->GetStunned();

            UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
            if (!AnimInstance) return;

            // 애니메이션 몽타주가 끝날 때 호출할 함수 바인딩
            FOnMontageEnded OnHitMontageEnded;
            OnHitMontageEnded.BindUObject(this, &APlayerCharacter::HandleHitMontageEnded);
            AnimInstance->Montage_SetEndDelegate(OnHitMontageEnded, HitMontage);
        }
    }
}

void APlayerCharacter::HandleHealed(float HealAmount)
{
    // C++ 코드 처리
    if (!IsValid(StatComponent)) return;

    if (HealingNiagara)
    {
        UNiagaraFunctionLibrary::SpawnSystemAttached(
            HealingNiagara,
            GetMesh(),                 // 부착할 대상 컴포넌트 (캐릭터 메쉬)
            FName(),
            FVector::ZeroVector,       // 상대 위치 오프셋
            FRotator::ZeroRotator,     // 상대 회전 오프셋
            EAttachLocation::KeepRelativeOffset,
            true
        );
    }
}

void APlayerCharacter::HandleDead(AController* DamagedInstigator)
{
    GetCharacterMovement()->StopMovementImmediately();
    GetCharacterMovement()->DisableMovement();

    // 입력 해제
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        DisableInput(PlayerController);
    }

    // 사망 몽타주 재생
    if (DeadMontage)
    {
        PlayAnimMontage(DeadMontage);
    }

    // 게임오버 처리
    if (AGunFireGameMode* GFGameMode = GetWorld()
        ? GetWorld()->GetAuthGameMode<AGunFireGameMode>()
        : nullptr)
    {
        GFGameMode->GameOver();
    }
}

void APlayerCharacter::HandleHitMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (Montage != HitMontage) return;
    if (!IsValid(CombatComponent)) return;

    CombatComponent->ClearActionState(ECombatActionState::Stunned);
}

void APlayerCharacter::HandleDashMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (Montage != DashMontage) return;

    FinishDash();

    if (IsValid(CombatComponent))
    {
        CombatComponent->ClearActionState(ECombatActionState::Dodging);
    }
}

void APlayerCharacter::TestHeal()
{
    if (StatComponent)
    {
        StatComponent->Heal(20.0f);
    }
}

void APlayerCharacter::LockOn(const FInputActionValue& Value)
{
    if (!Controller) return;

    if (!IsLockOn)
    {
        IsLockOn = true;

        if (IsAiming) return;

        GetCharacterMovement()->bOrientRotationToMovement = false; // 이동 방향 회전 끄기
        bUseControllerRotationYaw = true;
    }
    else
    {
        IsLockOn = false;

        if (IsAiming) return;

        GetCharacterMovement()->bOrientRotationToMovement = true; // 이동 방향 회전
        bUseControllerRotationYaw = false;
    }
}

void APlayerCharacter::KillEnemyForDebug()
{
    // 테스트로 현재 방의 적을 처치함
    if (AGunFireGameMode* GFGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<AGunFireGameMode>() : nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Kill ALL Enemy for Test"));
        GFGameMode->KillEnemyForTest();
    }
}

void APlayerCharacter::EnableRagdoll()
{
    // 이동 정지
    UCharacterMovementComponent* MovementComp = GetCharacterMovement();
    if (IsValid(MovementComp))
    {
        MovementComp->StopMovementImmediately();
        MovementComp->DisableMovement();
    }

    // 캡슐 충돌 끄기
    UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
    if (IsValid(CapsuleComp))
    {
        CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    USkeletalMeshComponent* SkeletalMeshComp = GetMesh();
    if (IsValid(SkeletalMeshComp))
    {
        // 래그돌 프로필 적용
        SkeletalMeshComp->SetCollisionProfileName(TEXT("Ragdoll"));
	    SkeletalMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

        // AnimBp가 포즈 덮어쓰는 문제 방지
        SkeletalMeshComp->bPauseAnims = true;

        // 물리 시뮬레이션 시작
        SkeletalMeshComp->SetSimulatePhysics(true);
        SkeletalMeshComp->WakeAllRigidBodies();

        SetLifeSpan(3.f);
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
