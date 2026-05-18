#include "Weapon/WeaponComponent.h"

#include "EnhancedInputSubsystems.h"
#include "GameFramework/Character.h"
#include "Weapon/GunBase.h"
#include "Weapon/MeleeWeaponBase.h"
#include "Weapon/WeaponBase.h"

UWeaponComponent::UWeaponComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    Weapons.Init(nullptr, static_cast<int32>(EWeaponSlot::Count));
    WeaponMappingContext = nullptr;
    MappingPriority = 1;
    bIsAddedMappingContext = false;
}

void UWeaponComponent::BeginPlay()
{
    Super::BeginPlay();
}

AWeaponBase* UWeaponComponent::EquipWeapon(TSubclassOf<AWeaponBase> WeaponClass, EWeaponSlot Slot)
{
    if (!WeaponClass) return nullptr;

    // 장착하려는 손에 이미 무기가 있다면 장착 해제
    if (HasWeapon(Slot))
    {
        UnEquipWeapon(Slot);
    }

    AWeaponBase* NewWeapon = SpawnWeapon(WeaponClass);
    if (!IsValid(NewWeapon)) return nullptr;

    ACharacter* OwnerCharacter = GetOwnerCharacter();
    USceneComponent* SceneComponent = GetAttachComponent();

    if (!IsValid(OwnerCharacter) || !IsValid(SceneComponent))
    {
        NewWeapon->Destroy();
        return nullptr;
    }

    NewWeapon->Equip(OwnerCharacter, SceneComponent, GetAttachSocketName(Slot));
    SetWeapon(Slot, NewWeapon);

    AddWeaponMappingContext();

    return NewWeapon;
}

void UWeaponComponent::UnEquipWeapon(EWeaponSlot Slot)
{
    AWeaponBase* Weapon = GetWeapon(Slot);
    if (!IsValid(Weapon)) return;

    Weapon->UnEquip();
    Weapon->Destroy();
    SetWeapon(Slot, nullptr);

    if (!HasWeapon(EWeaponSlot::LeftHand) && !HasWeapon(EWeaponSlot::RightHand))
    {
        RemoveWeaponMappingContext();
    }
}

AWeaponBase* UWeaponComponent::GetWeapon(EWeaponSlot Slot) const
{
    int32 Index = static_cast<int32>(Slot);
    return Weapons.IsValidIndex(Index) ? Weapons[Index] : nullptr;
}

AGunBase* UWeaponComponent::GetCurrentGun() const
{
    // 왼손 무기가 총으로 캐스팅 가능하면 반환
    if (AGunBase* Gun = Cast<AGunBase>(GetWeapon(EWeaponSlot::LeftHand)))
    {
        return Gun;
    }

    // 불가능하면 오른손 무기 총으로 캐스팅해서 반환
    return Cast<AGunBase>(GetWeapon(EWeaponSlot::RightHand));
}

AMeleeWeaponBase* UWeaponComponent::GetCurrentMeleeWeapon() const
{
    // 오른손 무기가 근접 무기로 캐스팅 가능하면 반환
    if (AMeleeWeaponBase* MeleeWeapon = Cast<AMeleeWeaponBase>(GetWeapon(EWeaponSlot::RightHand)))
    {
        return MeleeWeapon;
    }

    // 불가능하면 왼손 무기 근접 무기로 캐스팅해서 반환
    return Cast<AMeleeWeaponBase>(GetWeapon(EWeaponSlot::LeftHand));
}

bool UWeaponComponent::HasGun() const
{
    return IsValid(GetCurrentGun());
}

bool UWeaponComponent::HasMeleeWeapon() const
{
    return IsValid(GetCurrentMeleeWeapon());
}

bool UWeaponComponent::HasWeapon(EWeaponSlot Slot) const
{
    return IsValid(GetWeapon(Slot));
}

ACharacter* UWeaponComponent::GetOwnerCharacter() const
{
    return Cast<ACharacter>(GetOwner());
}

USceneComponent* UWeaponComponent::GetAttachComponent() const
{
    ACharacter* OwnerCharacter = GetOwnerCharacter();
    if (!IsValid(OwnerCharacter)) return nullptr;

    return OwnerCharacter->GetMesh();
}

AWeaponBase* UWeaponComponent::SpawnWeapon(TSubclassOf<AWeaponBase> WeaponClass)
{
    if (!WeaponClass) return nullptr;

    UWorld* World = GetWorld();
    if (!World) return nullptr;

    ACharacter* OwnerCharacter = GetOwnerCharacter();
    if (!IsValid(OwnerCharacter)) return nullptr;

    AWeaponBase* NewWeapon = World->SpawnActor<AWeaponBase>(WeaponClass, OwnerCharacter->GetActorTransform());

    return NewWeapon;
}

FName UWeaponComponent::GetAttachSocketName(EWeaponSlot Slot) const
{
    switch (Slot)
    {
    case EWeaponSlot::LeftHand:
        return TEXT("LeftGrip");

    case EWeaponSlot::RightHand:
        return TEXT("RightGrip");

    default:
        return NAME_None;
    }
}

void UWeaponComponent::SetWeapon(EWeaponSlot Slot, AWeaponBase* NewWeapon)
{
    int32 Index = static_cast<int32>(Slot);
    if (!Weapons.IsValidIndex(Index)) return;

    Weapons[Index] = NewWeapon;

    // 장비 장착 이벤트 호출
    if (IsValid(NewWeapon))
    {
        OnWeaponEquipped.Broadcast(Slot, NewWeapon);
    }
}

void UWeaponComponent::AddWeaponMappingContext()
{
    if (!WeaponMappingContext || bIsAddedMappingContext) return;

    ACharacter* OwnerCharacter = GetOwnerCharacter();
    if (!IsValid(OwnerCharacter)) return;

    APlayerController* PlayerController = Cast<APlayerController>(OwnerCharacter->GetController());
    if (!IsValid(PlayerController)) return;

    ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
    if (!LocalPlayer) return;

    UEnhancedInputLocalPlayerSubsystem* Subsystem =
        LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
    if (!Subsystem) return;

    bIsAddedMappingContext = true;
    Subsystem->AddMappingContext(WeaponMappingContext, MappingPriority);
}

void UWeaponComponent::RemoveWeaponMappingContext()
{
    if (!WeaponMappingContext || !bIsAddedMappingContext) return;

    ACharacter* OwnerCharacter = GetOwnerCharacter();
    if (!IsValid(OwnerCharacter)) return;

    APlayerController* PlayerController = Cast<APlayerController>(OwnerCharacter->GetController());
    if (!IsValid(PlayerController)) return;

    ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
    if (!LocalPlayer) return;

    UEnhancedInputLocalPlayerSubsystem* Subsystem =
        LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
    if (!Subsystem) return;

    bIsAddedMappingContext = false;
    Subsystem->RemoveMappingContext(WeaponMappingContext);
}
