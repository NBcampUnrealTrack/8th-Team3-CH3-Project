#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MeleeCombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUNFIRE_API UMeleeCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMeleeCombatComponent();

protected:
	virtual void BeginPlay() override;

};
