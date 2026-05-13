#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

// 어떤 공격을 실행할지를 선택할지 결정함

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUNFIRE_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();

protected:
	virtual void BeginPlay() override;


};
