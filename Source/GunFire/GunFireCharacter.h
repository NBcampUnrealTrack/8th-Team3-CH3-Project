#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "GunFireCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AGunFireCharacter : public ACharacter
{
	GENERATED_BODY()

	// 캐릭터 메쉬
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;

	// 카메라 (1인칭)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;
	
public:
	AGunFireCharacter();

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

    void Shot(const FInputActionValue& Value);
    void MeleeAttack(const FInputActionValue& Value);
    void Aiming(const FInputActionValue& Value);
    void StopAiming();
    void Reload(const FInputActionValue& Value);
    void Skill(const FInputActionValue& Value);
    void Interaction(const FInputActionValue& Value);

protected:
	// 폰 인터페이스 (컨트롤러 변경 알림)
	virtual void NotifyControllerChanged() override;
    // 폰 인터페이스 (입력 바인딩 처리 함수)
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

public:
	// Mesh1P 하위 객체를 반환
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
    // FirstPersonCameraComponent 하위 객체를 반환
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

