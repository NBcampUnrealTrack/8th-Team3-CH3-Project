#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RangedCombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUNFIRE_API URangedCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URangedCombatComponent();

protected:
	virtual void BeginPlay() override;

};
