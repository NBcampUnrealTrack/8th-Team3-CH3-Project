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

    // 마지막으로 생성한 최종 위치
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    FVector LastPopupLocation;

    // 새로운 위치에 생성되는 시간, N초동안 데미지 팝업 없으면 새로운 위치에 팝업
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    float NewPositionTime;

    // 일정 시간 지나면 새로운 위치에 생성되도록 하는 타이머
    FTimerHandle PopupTimerHandle;

    // 일정 시간이 지나 마지막으로 생성된 위치를 비우는 함수
    void ClearLastLocation();
};
