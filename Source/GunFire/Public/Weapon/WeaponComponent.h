#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"


// 무기 장착, 획득 담당

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUNFIRE_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWeaponComponent();

    void EquipWeapon();

protected:
	virtual void BeginPlay() override;

protected:



};
