#include "Weapon/GunBase.h"

#include "GameFramework/Character.h"
#include "GunFire/GunFireProjectile.h"
#include "Kismet/GameplayStatics.h"

AGunBase::AGunBase()
{
    DamageRate = 0.3f;

    ReloadSound = nullptr;
    ReloadAnimation = nullptr;
    MuzzleSocketName = TEXT("Muzzle");

    RoF = 1.f;
    CurrentAmmo = 0;
    RemainAmmo = 0;
    MaxReserveAmmo = 24;
    ReloadPerAmmo = 12;
    Range = 1000.f;

    bCanFire = true;
    bCanReload = true;
}

void AGunBase::BeginPlay()
{
    Super::BeginPlay();

    CurrentAmmo = ReloadPerAmmo;
    RemainAmmo = MaxReserveAmmo;
}

void AGunBase::Attack()
{
    if (!bCanFire || CurrentAmmo <= 0) return;

    Fire();
}

void AGunBase::Fire()
{
    bCanFire = false;

    // 총알 실제로 생성
    if (ProjectileClass)
    {
        UWorld* World = GetWorld();
        if (!World) return;

        // 생성 필요

        --CurrentAmmo;
    }

    GetWorldTimerManager().SetTimer(
        FireDelayTimer,
        this,
        &AGunBase::HandleFireDelay,
        1.f / RoF
        );
}

void AGunBase::Reload()
{
    // 현재 총이 전부 장전되어있으면 X, 남은 총알이 없으면 X
    if (!bCanReload || CurrentAmmo >= ReloadPerAmmo || RemainAmmo <= 0) return;

    bCanFire = false;
    bCanReload = false;

    PlayReloadAnimation();
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

TSubclassOf<AGunFireProjectile> AGunBase::GetProjectileClass() const
{
    return ProjectileClass;
}

float AGunBase::GetRange() const
{
    return Range;
}

void AGunBase::PlayReloadSound()
{
    if (!ReloadSound) return;

    UGameplayStatics::PlaySoundAtLocation(
        this,
        ReloadSound,
        GetActorLocation()
        );
}

void AGunBase::PlayReloadAnimation()
{
    if (!ReloadAnimation) return;

    ACharacter* OwnerCharacter = GetOwnerCharacter();
    if (!IsValid(OwnerCharacter)) return;

    OwnerCharacter->PlayAnimMontage(ReloadAnimation);
}

void AGunBase::HandleFireDelay()
{
    GetWorldTimerManager().ClearTimer(FireDelayTimer);

    bCanFire = true;
}

void AGunBase::HandleReloadComplete()
{
    // 필요한 총알만큼만 장전함
    // 필요한 총알보다 남은 총알이 적으면 있는 만큼만 장전
    int32 RequiredAmmo = ReloadPerAmmo - CurrentAmmo;
    int32 ActualReloadingAmmo = FMath::Min(RemainAmmo, RequiredAmmo);

    CurrentAmmo += ActualReloadingAmmo;
    RemainAmmo -= ActualReloadingAmmo;

    bCanFire = true;
    bCanReload = true;
}

