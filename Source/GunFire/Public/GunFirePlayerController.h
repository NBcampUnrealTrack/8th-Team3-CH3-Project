#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GunFirePlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class GUNFIRE_API AGunFirePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    AGunFirePlayerController();

    UFUNCTION(BlueprintCallable)
    void ShowGameOverUI();

    UFUNCTION(BlueprintCallable)
    void ShowLoadingScreen();

    UFUNCTION(BlueprintCallable)
    void HideLoadingScreen();

    UFUNCTION(BlueprintCallable)
    void GotoMainMenu();

    UFUNCTION(BlueprintImplementableEvent)
    void UpdateLoadingProgress(float Progress);

    virtual void BeginPlay() override;

public:
    // IMC 맵핑
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputMappingContext* DefaultMappingContext;

    // 점프 액션
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* JumpAction;

    // 이동 액션
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* MoveAction;

    // 시점 액션
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* LookAction;

    // 대시(회피) 액션
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* DashAction;

    // 달리기 액션
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* RunAction;

    // 사격 액션
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* ShotAction;

    // 근접 공격 액션
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* MeleeAttackAction;

    // 상호작용 액션
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* InteractionAction;

    // 재장전 액션
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* ReloadAction;

    // 스킬 액션
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* SkillAction;

    // 조준 액션
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* AimingAction;

    // 디버그용 몬스터 처치 액션
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* KillTestAction;

    // 록온 액션
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* LockOnAction;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UUserWidget> GameOverWidgetClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    TObjectPtr<UUserWidget> GameOverWidget;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UUserWidget> LoadingScreenWidgetClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    TObjectPtr<UUserWidget> LoadingScreenWidget;

protected:
    UFUNCTION(BlueprintImplementableEvent)
    void OnHideLoadingScreen();
};
