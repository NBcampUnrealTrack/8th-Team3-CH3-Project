#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"

class AGunBase;
class AMeleeWeaponBase;
class AWeaponBase;
class ACharacter;
class UInputMappingContext;


UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
    LeftHand    UMETA(DisplayName = "왼손"),
    RightHand   UMETA(DisplayName = "오른손"),
    Count       UMETA(Hidden)
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWeaponEquippedSignature, EWeaponSlot, Slot, AWeaponBase*, Weapon);

// 무기 관리 클래스

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUNFIRE_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWeaponComponent();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    AWeaponBase* EquipWeapon(TSubclassOf<AWeaponBase> WeaponClass, EWeaponSlot Slot);

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void UnEquipWeapon(EWeaponSlot Slot);

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void UnEquipAllWeapons();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void SetHiddenAllWeapons(bool bHidden);

    UFUNCTION(BlueprintPure, Category = "Weapon")
    AWeaponBase* GetWeapon(EWeaponSlot Slot) const;

    UFUNCTION(BlueprintPure, Category = "Weapon")
    AGunBase* GetCurrentGun() const;

    UFUNCTION(BlueprintPure, Category = "Weapon")
    AMeleeWeaponBase* GetCurrentMeleeWeapon() const;

    UFUNCTION(BlueprintPure, Category = "Weapon")
    bool HasGun() const;

    UFUNCTION(BlueprintPure, Category = "Weapon")
    bool HasMeleeWeapon() const;

    UFUNCTION(BlueprintPure, Category = "Weapon")
    bool HasWeapon(EWeaponSlot Slot) const;

public:
    // 무기 장착을 알리는 델리게이트
    // 총기 장착 시 총알 변화 이벤트 바인딩을 위한 델리게이트임
    UPROPERTY(BlueprintAssignable)
    FWeaponEquippedSignature OnWeaponEquipped;

protected:
	virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    TArray<TObjectPtr<AWeaponBase>> Weapons;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Input")
    TObjectPtr<UInputMappingContext> WeaponMappingContext;

    // IMC 매핑 우선순위
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Input")
    int32 MappingPriority;

    // IMC가 추가되었는지 확인하는 변수
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Input")
    bool bIsAddedMappingContext;

private:
    ACharacter* GetOwnerCharacter() const;
    USceneComponent* GetAttachComponent() const;
    AWeaponBase* SpawnWeapon(TSubclassOf<AWeaponBase> WeaponClass);

    FName GetAttachSocketName(EWeaponSlot Slot) const;
    void SetWeapon(EWeaponSlot Slot, AWeaponBase* NewWeapon);

    void AddWeaponMappingContext();
    void RemoveWeaponMappingContext();
};
