#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Portal.generated.h"

class UBoxComponent;
class UNiagaraComponent;

UCLASS()
class GUNFIRE_API APortal : public AActor
{
	GENERATED_BODY()

public:
	APortal();

protected:
    // 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
    TObjectPtr<USceneComponent> Scene;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
    TObjectPtr<UBoxComponent> Collision;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
    TObjectPtr<UNiagaraComponent> PortalEffect;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
    TObjectPtr<UNiagaraComponent> PortalDashEffect;


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
