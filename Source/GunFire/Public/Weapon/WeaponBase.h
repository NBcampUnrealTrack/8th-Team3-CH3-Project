#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

class ACharacter;

UCLASS()
class GUNFIRE_API AWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	AWeaponBase();

    UFUNCTION(BlueprintCallable)
    virtual void Attack();

    // 캐릭터와 붙일 부위를 받아서 무기를 장착하는 함수
    UFUNCTION(BlueprintCallable)
    void Equip(ACharacter* Character, USceneComponent* AttachComponent, FName SocketName);

    // 장착 해제
    UFUNCTION(BlueprintCallable)
    void UnEquip();

    UFUNCTION(BlueprintPure)
    UStaticMeshComponent* GetMesh() const;

    UFUNCTION(BlueprintPure)
    UAnimMontage* GetAttackAnimation() const;

    UFUNCTION(BlueprintPure)
    bool IsEquipped() const;

    // 부착된 캐릭터를 반환하는 함수
    UFUNCTION(BlueprintPure)
    ACharacter* GetOwnerCharacter() const;

    UFUNCTION(BlueprintPure)
    virtual float GetDamageRate() const;

    UFUNCTION(BlueprintPure)
    float GetStaminaCost() const;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Component")
    TObjectPtr<USceneComponent> Scene;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Component")
    TObjectPtr<UStaticMeshComponent> StaticMesh;

    // 공격 애니메이션
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    TObjectPtr<UAnimMontage> AttackAnimation;

    // 장착 상태인지
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    bool bEquipped;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Stamina")
    float StaminaCost;
};
