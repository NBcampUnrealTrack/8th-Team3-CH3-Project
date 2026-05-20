#pragma once

#include "CoreMinimal.h"
#include "Weapon/WeaponBase.h"
#include "GunBase.generated.h"

struct FGunSessionData;
class AGunFireProjectile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAmmoChangedSignature, int32, CurrentAmmo, int32, RemainAmmo);

UCLASS()
class GUNFIRE_API AGunBase : public AWeaponBase
{
	GENERATED_BODY()

public:
    AGunBase();

    virtual void BeginPlay() override;

    // 공격 처리 함수
    virtual void Attack() override;

    // 사격 가능한지 확인하는 함수
    UFUNCTION(BlueprintPure)
    bool CanFire() const;

    // 사격 시 총알이 남았는지 확인하는 함수
    UFUNCTION(BlueprintCallable)
    bool TryConsumeAmmo();

    // 재장전
    UFUNCTION(BlueprintCallable)
    virtual void Reload();

    // 재장전 가능한지 확인하는 함수
    UFUNCTION(BlueprintPure)
    bool CanReload() const;

    // 재장전 시작 처리
    UFUNCTION(BlueprintCallable)
    bool TryStartReload();

    // 재장전 실제 적용 처리
    // 애니메이션 중 탄창을 갈아끼는 타이밍에 호출
    UFUNCTION(BlueprintCallable)
    bool ApplyReload();

    // 재장전 애니메이션 종료 처리
    UFUNCTION(BlueprintCallable)
    void FinishReload();

    UFUNCTION(BlueprintCallable)
    void AddAmmo(int32 Amount);

    UFUNCTION(BlueprintPure)
    FTransform GetMuzzleTransform() const;

    UFUNCTION(BlueprintPure)
    USoundBase* GetReloadSound() const;

    UFUNCTION(BlueprintPure)
    UAnimMontage* GetReloadAnimation() const;

    UFUNCTION(BlueprintPure)
    int32 GetCurrentAmmo() const;

    UFUNCTION(BlueprintPure)
    int32 GetRemainAmmo() const;

    UFUNCTION(BlueprintPure)
    TSubclassOf<AGunFireProjectile> GetProjectileClass() const;

    UFUNCTION(BlueprintPure)
    float GetRange() const;

    UFUNCTION(BlueprintPure)
    float GetProjectileSpeed() const;

    // 게임 인스턴스에 저장된 정보로 보유 정보 복구하는 함수
    UFUNCTION(BlueprintCallable)
    void SetSessionData(const FGunSessionData& SessionData);

public:
    // 총알수 변할 때
    UPROPERTY(BlueprintAssignable)
    FAmmoChangedSignature OnAmmoChanged;

protected:
    // 사격 딜레이 처리 함수
    void HandleFireDelay();

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

    // 총알의 속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Gun")
    float ProjectileSpeed;

    // 사격 가능한지
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon|Gun")
    bool bCanFire;

    // 재장전 가능한지
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon|Gun")
    bool bCanReload;

    // 재장전 중인지
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Gun")
    bool bReloading;

    FTimerHandle FireDelayTimer;
};
