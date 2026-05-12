#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoorBase.generated.h"

UCLASS()
class GUNFIRE_API ADoorBase : public AActor
{
    GENERATED_BODY()

public:
    ADoorBase();

    virtual void BeginPlay() override;

    // cpp, 블루프린트에서 호출하는 함수
    UFUNCTION(BlueprintCallable)
    void OpenDoor();

    UFUNCTION(BlueprintCallable)
    void CloseDoor();

    // 실제 문의 열림을 처리할 함수
    UFUNCTION(BlueprintNativeEvent)
    void OnOpenDoor();

    // 실제 문의 닫힘을 처리할 함수
    UFUNCTION(BlueprintNativeEvent)
    void OnCloseDoor();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door|Component")
    TObjectPtr<USceneComponent> Scene;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door|Component")
    TObjectPtr<UStaticMeshComponent> StaticMesh;

};
