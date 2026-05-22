#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetingSystem.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUNFIRE_API UTargetingSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTargetingSystem();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
    float TraceDistance;
    /*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
    TArray<AActor*> */
protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    void FindAllTarget();
};
