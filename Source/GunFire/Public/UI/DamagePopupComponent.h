#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamagePopupComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class GUNFIRE_API UDamagePopupComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDamagePopupComponent();

protected:
	virtual void BeginPlay() override;

    UFUNCTION()
    void HandleDamaged(float ReceivedDamage, AController* DamageInstigator);

    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
    void ShowDamagePopup(float ReceivedDamage, AController* DamageInstigator, FVector SpawnLocation);

protected:
    // 생성할 기본 위치
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    FVector SpawnOffset;

    // 랜덤 반경
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    float RandomOffsetRadius;
};
