// Fill out your copyright notice in the Description page of Project Settings.


#include "ZoneEffectTrigger.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

AZoneEffectTrigger::AZoneEffectTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    SetRootComponent(TriggerBox);

    TriggerBox->SetBoxExtent(FVector(200.0f, 200.0f, 200.0f));

    TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
    TriggerBox->SetGenerateOverlapEvents(true);
}

void AZoneEffectTrigger::BeginPlay()
{
	Super::BeginPlay();

    if (TriggerBox)
    {
        TriggerBox->OnComponentBeginOverlap.AddDynamic(
            this, &AZoneEffectTrigger::OnTriggerBeginOverlap);
    }
}

void AZoneEffectTrigger::OnTriggerBeginOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (bTriggerOnce && bHasTriggered)
    {
        return;
    }

    if (!IsValid(OtherActor) || OtherActor == this)
    {
        return;
    }

    if (bOnlyTriggerByPawn && !OtherActor->IsA<APawn>())
    {
        return;
    }

    bHasTriggered = true;
    ApplyZoneEffect();
}

void AZoneEffectTrigger::ApplyZoneEffect()
{
    for (AActor* MeshActor : MeshActorsToHide)
    {
        if (!IsValid(MeshActor))
        {
            continue;
        }

        MeshActor->SetActorHiddenInGame(true);

        if (bDisableCollisionOnHide)
        {
            MeshActor->SetActorEnableCollision(false);
        }
    }

    for (AActor* PhysActor : PhysicsActorsToWake)
    {
        if (!IsValid(PhysActor))
        {
            continue;
        }

        TArray<UPrimitiveComponent*> PrimComps;
        PhysActor->GetComponents<UPrimitiveComponent>(PrimComps);

        for (UPrimitiveComponent* Prim : PrimComps)
        {
            if (Prim && Prim->IsSimulatingPhysics())
            {
                Prim->WakeAllRigidBodies();
            }
        }
    }

    if (SoundToPlay)
    {
        if (bPlaySoundAtLocation)
        {
            UGameplayStatics::PlaySoundAtLocation(this, SoundToPlay, GetActorLocation());
        }
        else
        {
            UGameplayStatics::PlaySound2D(this, SoundToPlay);
        }
    }
}

