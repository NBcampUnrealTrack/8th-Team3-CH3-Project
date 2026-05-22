#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RangedCombatComponent.generated.h"

class AGunBase;
class ACharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReloadFinishedSignature);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUNFIRE_API URangedCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URangedCombatComponent();

    bool TryFire(AGunBase* Gun, float AttackPower);

    bool TryReload(AGunBase* Gun);

    // AnimNotify에서 호출해서 실제 재장전을 적용하는 함수
    UFUNCTION(BlueprintCallable, Category = "Combat|Ranged")
    void ApplyReload();

    UFUNCTION(BlueprintCallable, Category = "Combat|Ranged")
    void FinishReload();

public:
    // 재장전 종료 상태를 알리는 델리게이트
    UPROPERTY(BlueprintAssignable, Category = "Combat|Ranged")
    FReloadFinishedSignature OnReloadFinished;

protected:
	virtual void BeginPlay() override;

protected:
    // 컴포넌트 소유자
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Ranged")
    TObjectPtr<ACharacter> OwnerCharacter;

private:
    // 카메라를 기준으로 조준점을 잡는 함수
    FVector GetCameraAimPoint(AGunBase* Gun) const;

    bool SpawnProjectile(AGunBase* Gun, float Damage);
    void PlayFireSound(AGunBase* Gun);
    void PlayFireAnimation(AGunBase* Gun);
    void PlayReloadSound(AGunBase* Gun);
    bool PlayReloadAnimation(AGunBase* Gun);

    // 재장전 몽타주 종료시 호출할 함수
    UFUNCTION()
    void HandleReloadMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:
    UPROPERTY()
    TWeakObjectPtr<AGunBase> CurrentReloadingGun;

    UPROPERTY()
    TObjectPtr<UAnimMontage> CurrentReloadingMontage;
};
