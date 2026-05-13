#pragma once

#include "CoreMinimal.h"
#include "Weapon/WeaponBase.h"
#include "GunBase.generated.h"

class AGunFireProjectile;

UCLASS()
class GUNFIRE_API AGunBase : public AWeaponBase
{
	GENERATED_BODY()

public:
    AGunBase();

    virtual void BeginPlay() override;

    // 공격 처리 함수
    virtual void Attack() override;

    // 재장전
    UFUNCTION(BlueprintCallable)
    virtual void Reload();

    UFUNCTION(BlueprintPure)
    FTransform GetMuzzleTransform() const;

    UFUNCTION(BlueprintPure)
    TSubclassOf<AGunFireProjectile> GetProjectileClass() const;

    UFUNCTION(BlueprintPure)
    float GetRange() const;

protected:
    // 실제 사격 처리
    virtual void Fire();

    // 재장전 소리 재생
    UFUNCTION(BlueprintCallable)
    void PlayReloadSound();

    // 재장전 애니메이션 몽타주 재생
    void PlayReloadAnimation();

    // 사격 딜레이 처리 함수
    void HandleFireDelay();

    // 재장전 애니메이션 중 실제 재장전 동작에서 호출될 함수
    UFUNCTION(BlueprintCallable)
    void HandleReloadComplete();

protected:
    // 발사할 총알 클래스
    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Gun")
    TSubclassOf<AGunFireProjectile> ProjectileClass;

    // 재장전 사운드
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Gun")
    TObjectPtr<USoundBase> ReloadSound;

    // 재장전 애니메이션
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Gun")
    TObjectPtr<UAnimMontage> ReloadAnimation;

    // 총구 소켓 이름
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Gun")
    FName MuzzleSocketName;

    // 다시 사격하는데 필요한 시간, 1.0f / Rof 로 처리할 것
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Gun")
    float RoF;

    // 현재 장전된 총알 수
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Gun")
    int32 CurrentAmmo;

    // 실제 보유하고 있는 남은 총알 수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Gun")
    int32 RemainAmmo;

    // 가지고 다닐 수 있는 최대 총알 수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Gun")
    int32 MaxReserveAmmo;

    // 재장전 한번에 채울 총알 수
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Gun")
    int32 ReloadPerAmmo;

    // 사거리
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Gun")
    float Range;

    // 사격 가능한지
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon|Gun")
    bool bCanFire;

    // 재장전 가능한지
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon|Gun")
    bool bCanReload;

    FTimerHandle FireDelayTimer;
};
