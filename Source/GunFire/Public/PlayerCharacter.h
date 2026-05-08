#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

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
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    USpringArmComponent* SpringArmComponent;
    // 카메라 (3인칭)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    UCameraComponent* ThirdPersonCameraComponent;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
    virtual void BeginPlay() override;

protected:
    // 대쉬(회피)
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Run", meta = (AllowPrivateAccess = "true"))
    bool bcanDash;                      // 대쉬 가능 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash", meta = (AllowPrivateAccess = "true"))
    float DashStrength;                 // 대쉬 세기
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash", meta = (AllowPrivateAccess = "true"))
    float DashCooldown;                 // 대쉬 쿨타임

    float DefaultGroundFriction;            // 현재 마찰력 저장
    float DefaultBrakingDeceleration;       // 현재 제동력 저장
    FTimerHandle DashCooldownTimerHandle;   // 대쉬 쿨타임타이머
    FTimerHandle DashStopTimerHandle;       // 대쉬 적용시간 타이머

    // 달리기
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run", meta = (AllowPrivateAccess = "true"))
    float NormalSpeed;          // 기본속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run", meta = (AllowPrivateAccess = "true"))
    float RunSpeedMultiplier;   // 기본 속도 대비 몇 배로 달리지 결정
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Run", meta = (AllowPrivateAccess = "true"))
    float RunSpeed;             // 최종 달리는 속도

    // 조준
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Aiming", meta = (AllowPrivateAccess = "true"))
    bool bIsAiming;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aiming", meta = (AllowPrivateAccess = "true"))
    float AimFOV;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aiming", meta = (AllowPrivateAccess = "true"))
    float DefaultFOV;
    // 조준 시 변경될 SocketOffset 값들
    FVector DefaultSocketOffset;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aiming", meta = (AllowPrivateAccess = "true"))
    FVector AimSocketOffset;

protected:
    // 이동
    void Move(const FInputActionValue& Value);
    // 시점
    void Look(const FInputActionValue& Value);
    // 대쉬(회피)
    void Dash(const FInputActionValue& Value);
    void StopDash();
    void ResetDash();       // 대쉬(회피) 초기화
    // 달리기
    void Run(const FInputActionValue& Value);
    void StopRun();
    // 사격
    void Shot(const FInputActionValue& Value);
    // 조준
    void Aiming(const FInputActionValue& Value);
    void StopAiming();
    // 재장전
    void Reload(const FInputActionValue& Value);
    // 근접 공격
    void MeleeAttack(const FInputActionValue& Value);
    // 스킬 사용
    void Skill(const FInputActionValue& Value);
    // 상호작용
    void Interaction(const FInputActionValue& Value);
};
