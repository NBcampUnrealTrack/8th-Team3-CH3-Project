#include "Weapon/WeaponBase.h"

#include "GameFramework/Character.h"


AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;

    Scene = CreateDefaultSubobject<USceneComponent>("Scene");
    SetRootComponent(Scene);

    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
    StaticMesh->SetCollisionProfileName(TEXT("NoCollision"));
    StaticMesh->SetupAttachment(Scene);

    AttackSound = nullptr;
    AttackAnimation = nullptr;
    bEquipped = false;
    StaminaCost = 0.f;
}

void AWeaponBase::Attack()
{
}

void AWeaponBase::Equip(ACharacter* Character, USceneComponent* AttachComponent, FName SocketName)
{
    if (!IsValid(Character) || !IsValid(AttachComponent) || IsEquipped()) return;

    SetOwner(Character);
    SetInstigator(Cast<APawn>(Character));
    bEquipped = true;

    if (IsValid(StaticMesh))
    {
        StaticMesh->SetVisibility(true);
    }

    AttachToComponent(
        AttachComponent,
        FAttachmentTransformRules::SnapToTargetNotIncludingScale,
        SocketName);
}

void AWeaponBase::UnEquip()
{
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    SetOwner(nullptr);
    SetInstigator(nullptr);
    bEquipped = false;

    if (IsValid(StaticMesh))
    {
        StaticMesh->SetVisibility(false);
    }
}

UStaticMeshComponent* AWeaponBase::GetMesh() const
{
    return StaticMesh.Get();
}

USoundBase* AWeaponBase::GetAttackSound() const
{
    return AttackSound.Get();
}

UAnimMontage* AWeaponBase::GetAttackAnimation() const
{
    return AttackAnimation.Get();
}

bool AWeaponBase::IsEquipped() const
{
    return bEquipped;
}

ACharacter* AWeaponBase::GetOwnerCharacter() const
{
    return Cast<ACharacter>(GetOwner());
}

float AWeaponBase::GetDamageRate() const
{
    UE_LOG(LogTemp, Warning, TEXT("Base GetDamageRate()"));
    return 1.f;
}

float AWeaponBase::GetStaminaCost() const
{
    return StaminaCost;
}
