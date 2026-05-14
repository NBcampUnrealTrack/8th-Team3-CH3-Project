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

    // cpp, 블루프린트에서 호출하는 이벤트, 문 열림
    UFUNCTION(BlueprintNativeEvent)
    void OpenDoor();

    // cpp, 블루프린트에서 호출하는 이벤트, 문 닫힘
    UFUNCTION(BlueprintNativeEvent)
    void CloseDoor();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door|Component")
    TObjectPtr<USceneComponent> Scene;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door|Component")
    TObjectPtr<UStaticMeshComponent> StaticMesh;

};
