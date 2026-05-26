#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactables/InteractableInterface.h"
#include "InteractTestItem.generated.h"

UCLASS()
class GUNFIRE_API AInteractTestItem : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	AInteractTestItem();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
    class UStaticMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    class UWidgetComponent* InteractionWidget;

    virtual void Interact_Implementation(AActor* Interactor) override;
    virtual void LookAt_Implementation() override;
    virtual void LookAway_Implementation() override;

};
