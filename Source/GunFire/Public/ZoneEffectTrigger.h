
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZoneEffectTrigger.generated.h"

class UBoxComponent;
class USoundBase;

UCLASS()
class GUNFIRE_API AZoneEffectTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	AZoneEffectTrigger();

protected:
	virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, Category = "Zone")
    TObjectPtr<UBoxComponent> TriggerBox;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone|Effect")
    TArray<TObjectPtr<AActor>> MeshActorsToHide;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone|Effect")
    TObjectPtr<USoundBase> SoundToPlay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone|Effect")
    bool bPlaySoundAtLocation = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone|Effect")
    bool bTriggerOnce = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone|Effect")
    bool bOnlyTriggerByPawn = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone|Effect")
    bool bDisableCollisionOnHide = true;


private:
    bool bHasTriggered = false;

    UFUNCTION()
    void OnTriggerBeginOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    void ApplyZoneEffect();
};
