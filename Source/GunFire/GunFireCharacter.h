#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "GunFireCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
//class UInputAction;
//class UInputMappingContext;
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

	//// IMC 맵핑
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	//UInputMappingContext* DefaultMappingContext;

	//// 점프 액션
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	//UInputAction* JumpAction;

	//// 이동 액션
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	//UInputAction* MoveAction;

	//// 시점 액션
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	//class UInputAction* LookAction;
	
public:
	AGunFireCharacter();
protected:
	// 이동
	void Move(const FInputActionValue& Value);

	// 시점
	void Look(const FInputActionValue& Value);

    // 대쉬
	void Dash(const FInputActionValue& Value);
    void StopDash();
    void ResetDash();
    bool bcanDash;                      // 대쉬 가능 여부
    float DashStrength;                 // 대쉬 세기
    float DashCooldown;                 // 대쉬 쿨타임
    float DefaultGroundFriction;        // 현재 마찰력 저장
    float DefaultBrakingDeceleration;   // 현재 제동력 저장
    FTimerHandle DashCooldownTimerHandle;
    FTimerHandle DashStopTimerHandle;

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

