#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class UStatComponent;
class UCombatComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputComponent;
struct FInputActionValue;

UCLASS()
class GUNFIRE_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

    // 캐릭터 메쉬
    //UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
    //USkeletalMeshComponent* SkeletalMesh;

    // 스프링 암 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    USpringArmComponent* SpringArmComponent;
    // 카메라 (3인칭)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* ThirdPersonCameraComponent;
public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // BeginPlay 전에 컴포넌트를 준비할 수 있는 함수
    virtual void PostInitializeComponents() override;


protected:

    // 대쉬(회피)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Run")
    bool CanDash;                      // 대쉬 가능 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
    float DashStrength;                 // 대쉬 세기
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
    float DashCooldown;                 // 대쉬 쿨타임
    float DefaultGroundFriction;            // 현재 마찰력 저장
    float DefaultBrakingDeceleration;       // 현재 제동력 저장
    FTimerHandle DashCooldownTimerHandle;   // 대쉬 쿨타임타이머
    FTimerHandle DashStopTimerHandle;       // 대쉬 적용시간 타이머

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


    // 근접 공격
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
    float HeavyAttackHoldTime;

    bool bHeavyAttackTriggered;

    FTimerHandle HeavyAttackTimerHandle;


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
    // 시점
    void Look(const FInputActionValue& Value);
    // 점프 오버라이드
    virtual void Jump() override;
    // 대쉬(회피)
    void Dash(const FInputActionValue& Value);
    void StopDash();
    void ResetDash();       // 대쉬(회피) 초기화
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

    // 체력 변경에 따른 HUD 갱신 처리
    UFUNCTION()
    void HandleHealthChanged(float CurrentHealth, float MaxHealth);

    // 스태미너 변경에 따른 HUD 갱신 처리
    UFUNCTION()
    void HandleStaminaChanged(float CurrentStamina, float MaxStamina);


    // 디버그용 몬스터 처치 함수
    void KillEnemyForDebug();

};
