#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamagePopupActor.generated.h"

class UWidgetComponent;

UCLASS()
class GUNFIRE_API ADamagePopupActor : public AActor
{
	GENERATED_BODY()

public:
	ADamagePopupActor();

    UFUNCTION(BlueprintCallable)
    void InitDamagePopupActor(float NewDamage);

protected:

    UFUNCTION(BlueprintImplementableEvent)
    void OnDamagePopupInitialized(float NewDamage);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    TObjectPtr<USceneComponent> Scene;

    // 위젯 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    TObjectPtr<UWidgetComponent> WidgetComponent;
};
