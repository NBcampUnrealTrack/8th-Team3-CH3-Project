#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RangedCombatComponent.generated.h"

class AGunBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReloadFinishedSignature);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUNFIRE_API URangedCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URangedCombatComponent();

    void TryFire(AGunBase* Gun, float AttackPower);

    bool TryReload(AGunBase* Gun);

    UFUNCTION(BlueprintCallable, Category = "Combat|Ranged")
    void FinishReload();

public:
    UPROPERTY(BlueprintAssignable, Category = "Combat|Ranged")
    FReloadFinishedSignature OnReloadFinished;

protected:
	virtual void BeginPlay() override;

};
