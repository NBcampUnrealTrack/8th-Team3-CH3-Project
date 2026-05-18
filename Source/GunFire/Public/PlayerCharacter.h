#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

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

    // 애니메이션 몽타주
    // 대쉬
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
    UAnimMontage* DashMontage;
    // 사격
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shot")
    UAnimMontage* FireMontage;
    // 사격 딜레이
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shot")
    UAnimMontage* FireDelayMontage;
    // 재장전
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reload")
    UAnimMontage* ReloadMontage;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UFUNCTION(BlueprintPure, Category = "Stats")
    int32 GetCurrentHealth() const;
    UFUNCTION(BlueprintPure, Category = "Stats")
    int32 GetCurrentStamina() const;
    UFUNCTION(BlueprintCallable, Category = "Stats")
    void SetCurrentHealth(int32 Amount);
    UFUNCTION(BlueprintCallable, Category = "Stats")
    void SetCurrentStamina(int32 Amount);

    UFUNCTION(BlueprintPure, Category = "Stats")
    int32 GetMaxHealth() const;
    UFUNCTION(BlueprintPure, Category = "Stats")
    int32 GetMaxStamina() const;
    UFUNCTION(BlueprintCallable, Category = "Stats")
    void SetMaxHealth(int32 Amount);
    UFUNCTION(BlueprintCallable, Category = "Stats")
    void SetMaxStamina(int32 Amount);

protected:

    // 대쉬(회피)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dash")
    bool CanDash;                      // 대쉬 가능 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
    float DashStrength;                 // 대쉬 세기
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
    float DashCooldown;                 // 대쉬 쿨타임
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
    float DashDuration;                 // 대쉬 지속시간
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

    // 사격
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shot")
    int32 CurrentAmmo;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shot")
    int32 MaxAmmo;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shot")
    int32 TotalAmmo;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shot")
    int32 AmmoPerFire;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shot")
    bool CanFire;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shot")
    float Rof;
    FTimerHandle ShotDelayTimerHandle;

    //재장전
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reload")
    bool IsReloading;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reload")
    float ReloadTime;
    FTimerHandle ReloadTimerHandle;

    // 상호작용
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
    AActor* TargetedActor;  // 상호작용 가능한 액터 저장
    FTimerHandle InteractionCheckTimerHandle;

    // 스태미나
    FTimerHandle NaturalHealingStaminaTimerHandle;


    // 근접 공격
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
    float HeavyAttackHoldTime;

    bool bHeavyAttackTriggered;

    FTimerHandle HeavyAttackTimerHandle;


    // 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
    TObjectPtr<UCombatComponent> CombatComponent;

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
    void ShotDelay();
    // 재장전
    void Reload(const FInputActionValue& Value);
    void Reloading();
    // 근접 공격
    void MeleeAttack(const FInputActionValue& Value);
    // 스킬 사용
    void Skill(const FInputActionValue& Value);
    // 상호작용
    void Interaction(const FInputActionValue& Value);
    void CheckForInteractables();   // 상호작용 트레이스 방식
    void CheckInteractablesRamge(); // 범위감지 방식

    // 디버그용 몬스터 처치 함수
    void KillEnemyForDebug();

    void MeleeAttackStarted();
    void MeleeAttackReleased();
    void OnHeavyAttack();

    // 디버그용 데미지, 힐
    UFUNCTION(BlueprintCallable)
    void DamageForDebug();
    UFUNCTION(BlueprintCallable)
    void AddHealthForDebug();
    void NaturalHealingStamina();
private:
    UPROPERTY(VisibleAnywhere, Category = "Stats")
    int32 CurrentHealth;
    UPROPERTY(VisibleAnywhere, Category = "Stats")
    int32 MaxHealth;
    UPROPERTY(VisibleAnywhere, Category = "Stats")
    int32 CurrentStamina;
    UPROPERTY(VisibleAnywhere, Category = "Stats")
    int32 MaxStamina;
};
