#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class UAnimMontage;
class UStatComponent;
class UCombatComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputComponent;
class UParticleSystem;
class UParticleSystemComponent;
class UNiagaraSystem;
class UNiagaraComponent;
struct FInputActionValue;

UCLASS()
class GUNFIRE_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

    UFUNCTION(BlueprintCallable)
    bool HasMovementInput() const;

    // 공격할 때 회전 방향을 구하는 함수
    UFUNCTION(BlueprintCallable)
    FRotator GetAttackInputRotation() const;

    // 래그돌 적용하는 함수
    UFUNCTION(BlueprintCallable)
    void EnableRagdoll();

    // 캐릭터 메쉬
    //UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
    //USkeletalMeshComponent* SkeletalMesh;

    // 스프링 암 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    USpringArmComponent* SpringArmComponent;
    // 카메라 (3인칭)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* ThirdPersonCameraComponent;

    UFUNCTION(BlueprintPure)
    bool CanStartDash();

    // 대쉬 시작
    UFUNCTION(BlueprintCallable)
    bool StartDash(float DashStrength);

    // 대쉬 종료
    UFUNCTION(BlueprintCallable)
    void FinishDash();

    // 대쉬 몽타주
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
    UAnimMontage* DashMontage;

    // 대쉬 이펙트
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    UParticleSystem* DashParticle;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    UNiagaraSystem* DashNiagara;
    UPROPERTY()
    UNiagaraComponent* ActiveDashNiagaraComp;
    // 몸에 붙여서 따라오게 할 때, 나중에 끄기 위해 기억해두는 컴포넌트 변수
    UPROPERTY()
    UParticleSystemComponent* ActiveDashParticleComp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    UNiagaraSystem* HealingNiagara;
public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // BeginPlay 전에 컴포넌트를 준비할 수 있는 함수
    virtual void PostInitializeComponents() override;

protected:

    // 대쉬(회피)
    float DefaultGroundFriction;            // 현재 마찰력 저장
    float DefaultBrakingDeceleration;       // 현재 제동력 저장
    bool bPrevOrientRotationToMovement;     // 대쉬 전 캐릭터 회전 상태
    bool bPrevUseControllerRotationYaw;     // 대쉬 전 캐릭터의 카메라 회전 상태
    bool bPrevUseControllerDesiredRotation;

    // 달리기
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run")
    float NormalSpeed;          // 기본속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run")
    float RunSpeedMultiplier;   // 기본 속도 대비 몇 배로 달리지 결정
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Run")
    float RunSpeed;             // 최종 달리는 속도

    // 조준
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aiming")
    bool IsAiming;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aiming")
    float AimFOV;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aiming")
    float DefaultFOV;
    // 조준 시 변경될 SocketOffset 값들
    FVector DefaultSocketOffset;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aiming")
    FVector AimSocketOffset;


    // 상호작용
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
    AActor* TargetedActor;  // 상호작용 가능한 액터 저장
    FTimerHandle InteractionCheckTimerHandle;

    // 록온
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reload")
    bool IsLockOn;

    // 스태미나
    FTimerHandle NaturalHealingStaminaTimerHandle;

    // 이동 입력 방향 저장하는 변수 (공격 시 방향전환에 사용)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
    FVector2D CurrentMovementInput;

    /* 근접 공격 변수 */

    // 강공격 판정 시간
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
    float HeavyAttackHoldTime;
    // 강공격 이미 발동했는지 체크하는 변수
    bool bHeavyAttackTriggered;

    FTimerHandle HeavyAttackTimerHandle;


    /* 애니메이션 몽타주 */

    // 사망 애니메이션 몽타주
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    TObjectPtr<UAnimMontage> DeadMontage;

    // 피격 애니메이션 몽타주
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    TObjectPtr<UAnimMontage> HitMontage;


    /* 컴포넌트 */

    // 전투 관리 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
    TObjectPtr<UCombatComponent> CombatComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
    TObjectPtr<UStatComponent> StatComponent;

protected:
    virtual void BeginPlay() override;

    // 이동
    void Move(const FInputActionValue& Value);
    void StopMoveInput();
    // 시점
    void Look(const FInputActionValue& Value);
    // 점프 오버라이드
    virtual void Jump() override;
    // 대쉬(회피)
    void Dash(const FInputActionValue& Value);
    // 달리기
    void Run(const FInputActionValue& Value);
    void StopRun();
    // 조준
    void Aiming(const FInputActionValue& Value);
    void StopAiming();
    // 사격
    void Shot(const FInputActionValue& Value);
    // 재장전
    void Reload(const FInputActionValue& Value);
    // 스킬 사용
    void Skill(const FInputActionValue& Value);
    // 상호작용
    void Interaction(const FInputActionValue& Value);
    void CheckForInteractables();   // 상호작용 트레이스 방식
    void CheckInteractablesRamge(); // 범위감지 방식

    // 록온
    void LockOn(const FInputActionValue& Value);
    // 디버그용 몬스터 처치 함수
    void KillEnemyForDebug();

    /* 근접 공격 처리 */


    void MeleeAttackStarted();
    void MeleeAttackReleased();
    void OnHeavyAttack();


    /* 이벤트 핸들 함수 */

    // 피격 이벤트 처리, 이펙트, 애니메이션, 사운드 등
    UFUNCTION()
    void HandleDamaged(float ActualDamage, AController* DamagedInstigator);

    // 회복 이벤트 처리, 이펙트, 사운드 등
    UFUNCTION()
    void HandleHealed(float HealAmount);

    // 사망 이벤트 처리
    UFUNCTION()
    void HandleDead(AController* DamagedInstigator);

    // 피격 몽타주 종료시 처리
    void HandleHitMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    void HandleDashMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};
