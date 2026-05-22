#pragma once

#include "CoreMinimal.h"
#include "Damageable.h"
#include "GameFramework/Actor.h"
#include "BreakableObject.generated.h"

UCLASS()
class GUNFIRE_API ABreakableObject : public AActor, public IDamageable
{
	GENERATED_BODY()

public:
	ABreakableObject();

protected:


};
