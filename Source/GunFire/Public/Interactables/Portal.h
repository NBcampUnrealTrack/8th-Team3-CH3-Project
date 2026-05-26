#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Portal.generated.h"

class UBoxComponent;
class UNiagaraComponent;

UENUM(BlueprintType)
enum class EPortalType : uint8
{
    NextFloor,
    ClearGame
};

UCLASS()
class GUNFIRE_API APortal : public AActor
{
	GENERATED_BODY()

public:
	APortal();

    virtual void BeginPlay() override;

    void SetActive(bool NewValue);

protected:
    // 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Portal|Component")
    TObjectPtr<USceneComponent> Scene;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Portal|Component")
    TObjectPtr<UBoxComponent> Collision;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Portal|Component")
    TObjectPtr<UNiagaraComponent> PortalEffect;

    // 넘어갈 다음 레벨의 이름
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portal")
    FName TargetLevelName;

    // 활성화 상태
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal")
    bool bActive;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portal")
    EPortalType PortalType;

protected:
    // 충돌 바인딩 함수
    UFUNCTION()
    void OnPortalBeginOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
        );

};
