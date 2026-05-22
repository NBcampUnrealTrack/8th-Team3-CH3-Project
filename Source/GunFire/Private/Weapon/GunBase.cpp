#include "Weapon/GunBase.h"

#include "Game/SessionData.h"
#include "GunFire/GunFireProjectile.h"

AGunBase::AGunBase()
{
    ReloadSound = nullptr;
    ReloadAnimation = nullptr;
    MuzzleSocketName = TEXT("Muzzle");

    RoF = 1.f;
    CurrentAmmo = 0;
    RemainAmmo = 0;
    MaxReserveAmmo = 24;
    ReloadPerAmmo = 12;
    Range = 1000.f;
    ProjectileSpeed = 6000.f;

    bCanFire = true;
    bCanReload = true;
    bReloading = false;

    DamageRate = 0.3f;
}

void AGunBase::BeginPlay()
{
    Super::BeginPlay();

    CurrentAmmo = ReloadPerAmmo;
    RemainAmmo = MaxReserveAmmo;
}

void AGunBase::Attack()
{
    TryConsumeAmmo();
}

bool AGunBase::CanFire() const
{
    return bCanFire && !bReloading && CurrentAmmo > 0;
}

bool AGunBase::TryConsumeAmmo()
{
    if (!CanFire()) return false;

    --CurrentAmmo;
    bCanFire = false;

    OnAmmoChanged.Broadcast(CurrentAmmo, RemainAmmo);

    const float FireInterval = RoF > 0.f
        ? 1.f / RoF
        : 0.1f;

    GetWorldTimerManager().SetTimer(
        FireDelayTimer,
        this,
        &AGunBase::HandleFireDelay,
        FireInterval,
        false
        );

    return true;
}

void AGunBase::Reload()
{
    TryStartReload();
}

bool AGunBase::CanReload() const
{
    return bCanReload
        && !bReloading
        && CurrentAmmo < ReloadPerAmmo
        && RemainAmmo > 0;
}

bool AGunBase::TryStartReload()
{
    if (!CanReload()) return false;

    bCanFire = false;
    bCanReload = false;
    bReloading = true;

    return true;
}

bool AGunBase::ApplyReload()
{
    if (!bReloading) return false;

    // 현재 총알이 가득 차있거나, 남은 총알이 없다면 false 리턴
    if (CurrentAmmo >= ReloadPerAmmo) return false;
    if (RemainAmmo <= 0) return false;

    // 필요한 총알만큼만 장전함
    // 필요한 총알보다 남은 총알이 적으면 있는 만큼만 장전
    int32 RequiredAmmo = ReloadPerAmmo - CurrentAmmo;
    int32 ActualReloadingAmmo = FMath::Min(RemainAmmo, RequiredAmmo);

    CurrentAmmo += ActualReloadingAmmo;
    RemainAmmo -= ActualReloadingAmmo;

    OnAmmoChanged.Broadcast(CurrentAmmo, RemainAmmo);

    return true;
}

void AGunBase::FinishReload()
{
    if (!bReloading) return;

    bCanFire = true;
    bCanReload = true;
    bReloading = false;
}

void AGunBase::AddAmmo(int32 Amount)
{
    if (RemainAmmo >= MaxReserveAmmo) return;

    RemainAmmo = FMath::Min(RemainAmmo + Amount, MaxReserveAmmo);

    OnAmmoChanged.Broadcast(CurrentAmmo, RemainAmmo);
}

FTransform AGunBase::GetMuzzleTransform() const
{
    UStaticMeshComponent* Mesh = GetMesh();

    // 메시가 없거나, 소켓이름이 없거나, 해당 소켓이 없다면 액터 트랜스폼 반환
    if (!IsValid(Mesh) || MuzzleSocketName.IsNone() || !Mesh->DoesSocketExist(MuzzleSocketName))
    {
        return GetActorTransform();
    }

    return Mesh->GetSocketTransform(MuzzleSocketName);
}

USoundBase* AGunBase::GetReloadSound() const
{
    return ReloadSound.Get();
}

UAnimMontage* AGunBase::GetReloadAnimation() const
{
    return ReloadAnimation.Get();
}

int32 AGunBase::GetCurrentAmmo() const
{
    return CurrentAmmo;
}

int32 AGunBase::GetRemainAmmo() const
{
    return RemainAmmo;
}

TSubclassOf<AGunFireProjectile> AGunBase::GetProjectileClass() const
{
    return ProjectileClass;
}

float AGunBase::GetRange() const
{
    return Range;
}

float AGunBase::GetProjectileSpeed() const
{
    return ProjectileSpeed;
}

void AGunBase::SetSessionData(const FGunSessionData& SessionData)
{
    CurrentAmmo = SessionData.CurrentAmmo;
    RemainAmmo = SessionData.RemainAmmo;

    OnAmmoChanged.Broadcast(CurrentAmmo, RemainAmmo);
}

float AGunBase::GetDamageRate() const
{
    return DamageRate;
}

void AGunBase::HandleFireDelay()
{
    GetWorldTimerManager().ClearTimer(FireDelayTimer);

    bCanFire = true;
}
