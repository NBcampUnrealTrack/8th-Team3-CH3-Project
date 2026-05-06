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

    // 대시 액션
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* DashAction;

    

    virtual void BeginPlay() override;
};
