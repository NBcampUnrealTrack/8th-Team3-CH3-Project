#include "Combat/RangedCombatComponent.h"

#include "Animation/AnimInstance.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon/GunBase.h"
#include "GunFire/GunFireProjectile.h"

URangedCombatComponent::URangedCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

    OwnerCharacter = nullptr;
    CurrentReloadingGun.Reset();
    CurrentReloadingMontage = nullptr;
}

void URangedCombatComponent::BeginPlay()
{
    Super::BeginPlay();

    AActor* Owner = GetOwner();
    if (!IsValid(Owner)) return;

    OwnerCharacter = Cast<ACharacter>(Owner);
}

bool URangedCombatComponent::TryFire(AGunBase* Gun, float AttackPower)
{
    if (!IsValid(OwnerCharacter) || !IsValid(Gun)) return false;

    if (!Gun->CanFire()) return false;
    if (!Gun->GetProjectileClass()) return false;

    float ActualDamage = AttackPower * Gun->GetDamageRate();

    // 총알 소모할 수 있는지 확인하고 총알 생성
    if (!Gun->TryConsumeAmmo()) return false;
    if (!SpawnProjectile(Gun, ActualDamage)) return false;

    PlayFireSound(Gun);
    PlayFireAnimation(Gun);

    return true;
}

bool URangedCombatComponent::TryReload(AGunBase* Gun)
{
    if (!IsValid(Gun)) return false;
    if (!Gun->TryStartReload()) return false;

    CurrentReloadingGun = Gun;

    // 재장전 애니메이션 실행 시도
    if (!PlayReloadAnimation(Gun))
    {
        ApplyReload();
        FinishReload();
    }

    return true;
}

void URangedCombatComponent::ApplyReload()
{
    if (!CurrentReloadingGun.IsValid()) return;

    AGunBase* Gun = CurrentReloadingGun.Get();
    if (!IsValid(Gun)) return;

    if (Gun->ApplyReload())
    {
        PlayReloadSound(Gun);
    }
}

void URangedCombatComponent::FinishReload()
{
    if (CurrentReloadingGun.IsValid())
    {
        CurrentReloadingGun->FinishReload();
        CurrentReloadingGun.Reset();
    }

    CurrentReloadingMontage = nullptr;

    OnReloadFinished.Broadcast();
}

FVector URangedCombatComponent::GetCameraAimPoint(AGunBase* Gun) const
{
    if (!IsValid(OwnerCharacter) || !IsValid(Gun))
    {
        return FVector::ZeroVector;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return FVector::ZeroVector;
    }

    AController* Controller = OwnerCharacter->GetController();
    if (!Controller)
    {
        // 컨트롤러가 유효하지 않으면 총구를 기준으로 사거리만큼 떨어진 곳의 위치를 가져옴
        FTransform MuzzleTransform = Gun->GetMuzzleTransform();
        return MuzzleTransform.GetLocation()
            + MuzzleTransform.GetUnitAxis(EAxis::X) * Gun->GetRange();
    }

    FVector ViewLocation;
    FRotator ViewRotation;

    // 현재 화면에 보이는 카메라의 위치, 회전값을 가져옴
    Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);

    // 조준점까지의 거리, 가능한 멀리 잡아야 크로스 헤어랑 비슷하게 됨
    constexpr float AimTraceDistance = 10000.f;

    // 카메라 중앙을 기준으로 먼 지점을 AimPoint로 잡음
    FVector TraceStart = ViewLocation;
    FVector TraceEnd = TraceStart + ViewRotation.Vector() * AimTraceDistance;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(OwnerCharacter);
    Params.AddIgnoredActor(Gun);

    FHitResult HitResult;
    bool bHit = World->LineTraceSingleByChannel(
        HitResult,
        TraceStart,
        TraceEnd,
        ECC_Visibility,
        Params
        );

    if (bHit)
    {
        return HitResult.ImpactPoint;
    }

    return TraceEnd;
}

bool URangedCombatComponent::SpawnProjectile(AGunBase* Gun, float Damage)
{
    if (!IsValid(OwnerCharacter) || !IsValid(Gun)) return false;

    UWorld* World = GetWorld();
    if (!World) return false;

    TSubclassOf<AGunFireProjectile> ProjectileClass = Gun->GetProjectileClass();
    if (!ProjectileClass) return false;

    FTransform MuzzleTransform = Gun->GetMuzzleTransform();
    FVector SpawnLocation = MuzzleTransform.GetLocation();
    FVector AimTarget = GetCameraAimPoint(Gun);

    FVector FireDirection = (AimTarget - SpawnLocation).GetSafeNormal();
    if (FireDirection.IsNearlyZero())
    {
        FireDirection = MuzzleTransform.GetUnitAxis(EAxis::X);
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = OwnerCharacter;
    SpawnParams.Instigator = OwnerCharacter;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AGunFireProjectile* Projectile = World->SpawnActor<AGunFireProjectile>(
        ProjectileClass,
        SpawnLocation,
        FireDirection.Rotation(),
        SpawnParams
        );

    if (!IsValid(Projectile)) return false;


    // 속도가 0이면 수명 설정에서 분모가 0이 되므로 1.f로 조정
    float ProjectileSpeed = FMath::Max(Gun->GetProjectileSpeed(), 1.f);
    float Range = FMath::Max(Gun->GetRange(), 1.f);

    // 총알에 가해자, 데미지, 속도, 사거리를 지정함
    Projectile->InitializeProjectile(OwnerCharacter->GetController(), OwnerCharacter,
        Damage, Range, ProjectileSpeed);

    // ----------  디버그 라인 그리기 -----------
#if ENABLE_DRAW_DEBUG
    DrawDebugLine(
        World,
        SpawnLocation,
        SpawnLocation + FireDirection * Gun->GetRange(),
        FColor::Green,
        false,
        1.f,
        0,
        1.f
        );
#endif
    // ----------  디버그 라인 그리기 -----------

    return true;
}

void URangedCombatComponent::PlayFireSound(AGunBase* Gun)
{
    if (!IsValid(Gun)) return;

    if (USoundBase* AttackSound = Gun->GetAttackSound())
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            AttackSound,
            Gun->GetActorLocation()
            );
    }
}

void URangedCombatComponent::PlayFireAnimation(AGunBase* Gun)
{
    if (!IsValid(OwnerCharacter) || !IsValid(Gun)) return;

    if (UAnimMontage* AttackMontage = Gun->GetAttackAnimation())
    {
        OwnerCharacter->PlayAnimMontage(AttackMontage);
    }
}

void URangedCombatComponent::PlayReloadSound(AGunBase* Gun)
{
    if (!IsValid(Gun)) return;

    if (USoundBase* ReloadSound = Gun->GetReloadSound())
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            ReloadSound,
            Gun->GetActorLocation()
            );
    }
}

bool URangedCombatComponent::PlayReloadAnimation(AGunBase* Gun)
{
    if (!IsValid(OwnerCharacter) || !IsValid(Gun)) return false;

    UAnimMontage* ReloadMontage = Gun->GetReloadAnimation();
    if (!ReloadMontage) return false;

    float Duration = OwnerCharacter->PlayAnimMontage(ReloadMontage);
    if (Duration < 0.0f) return false;

    UAnimInstance* AnimInstance = OwnerCharacter->GetMesh() ? OwnerCharacter->GetMesh()->GetAnimInstance() : nullptr;
    if (!AnimInstance) return false;

    CurrentReloadingMontage = ReloadMontage;

    // 몽타주 종료시 호출할 함수 등록
    FOnMontageEnded OnReloadMontageEnded;
    OnReloadMontageEnded.BindUObject(this, &URangedCombatComponent::HandleReloadMontageEnded);
    AnimInstance->Montage_SetEndDelegate(OnReloadMontageEnded);

    return true;
}

void URangedCombatComponent::HandleReloadMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (!CurrentReloadingGun.IsValid()) return;
    if (Montage != CurrentReloadingMontage) return;

    FinishReload();
}
