#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoorBase.generated.h"

class UBoxComponent;
class UArrowComponent;
class ADoorBase;

// 플레이어가 이 문을 통한 진입(보간)을 완료했을 때 브로드캐스트된다.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDoorEntered, ADoorBase*, Door);

UCLASS()
class GUNFIRE_API ADoorBase : public AActor
{
    GENERATED_BODY()

public:
    ADoorBase();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintNativeEvent)
    void OpenDoor();

    UFUNCTION(BlueprintNativeEvent)
    void CloseDoor();

    UFUNCTION(BlueprintCallable, Category = "Door")
    void TryEnter();

    // F 상호작용 잠금/해제.
    // 잠긴 동안에는 트리거에 들어와도 "F - 진입" 프롬프트가 뜨지 않고 TryEnter도 막힌다.
    // CloseDoor와 달리 이 잠금은 다시 풀 수 있다(SetInteractionLocked(false)).
    // CloseDoor : "전투 중 통과 차단" (영구). SetInteractionLocked : "조건부 상호작용 게이트" (가역).
    UFUNCTION(BlueprintCallable, Category = "Door")
    void SetInteractionLocked(bool bNewLocked);

    UFUNCTION(BlueprintPure, Category = "Door")
    bool IsInteractionLocked() const { return bLocked; }

    // 진입 보간이 완료된 시점에 브로드캐스트된다. (StartRoom 등이 바인딩해서 후처리)
    UPROPERTY(BlueprintAssignable, Category = "Door")
    FOnDoorEntered OnDoorEntered;

protected:
    UFUNCTION(BlueprintImplementableEvent, Category = "Door")
    void OnPlayerInRange(bool bInRange);

    UFUNCTION(BlueprintImplementableEvent, Category = "Door")
    void OnEntryStarted();

    UFUNCTION(BlueprintImplementableEvent, Category = "Door")
    void OnEntryFinished();

    UFUNCTION()
    void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
        const FHitResult& SweepResult);

    UFUNCTION()
    void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door|Component")
    TObjectPtr<USceneComponent> Scene;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door|Component")
    TObjectPtr<UStaticMeshComponent> StaticMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door|Component")
    TObjectPtr<UBoxComponent> TriggerVolume;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door|Component")
    TObjectPtr<UArrowComponent> EntryDirectionArrow;

    UPROPERTY(EditAnywhere, Category = "Door|Entry")
    float EntryDistance = 250.f;

    UPROPERTY(EditAnywhere, Category = "Door|Entry")
    float EntryDuration = 2.5f;

private:
    void FinishEntry();

    // 현재 트리거 안에 있는 플레이어에게 F 입력/프롬프트를 활성화한다.
    // OnTriggerBeginOverlap(잠금 해제 상태)과 SetInteractionLocked(false) 양쪽에서 사용.
    void EnableInteractionForCurrentPawn();

    UPROPERTY()
    TObjectPtr<APawn> OverlappingPawn;

    bool bHasEntered = false;
    bool bIsEntering = false;
    bool bInputBound = false;
    bool bLocked = false;       // F 상호작용 잠금 상태

    float EntryElapsed = 0.f;
    FVector EntryStartLocation = FVector::ZeroVector;
    FVector EntryTargetLocation = FVector::ZeroVector;
};
