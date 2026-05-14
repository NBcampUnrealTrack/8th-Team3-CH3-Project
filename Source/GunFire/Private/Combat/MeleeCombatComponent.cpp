#include "Combat/MeleeCombatComponent.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon/MeleeWeaponBase.h"
#include "DrawDebugHelpers.h"
#include "Enemy/EnemyBase.h"

UMeleeCombatComponent::UMeleeCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

    OwnerCharacter = nullptr;
    CurrentComboIndex = 0;

    bFinishedAttackForTest = true;
    CurrentPower = 0.f;
    AttackTraceInterval = 0.01f;
}

void UMeleeCombatComponent::BeginPlay()
{
    Super::BeginPlay();

    AActor* Owner = GetOwner();
    if (!IsValid(Owner)) return;

    OwnerCharacter = Cast<ACharacter>(Owner);
}

bool UMeleeCombatComponent::CanStartAttack(AMeleeWeaponBase* MeleeWeapon)
{
    if (!IsValid(MeleeWeapon)) return false;
    if (!IsValid(OwnerCharacter)) return false;

    // 현재 콤보에 해당하는 애니메이션 가져와서 있는지 판단하기
    UAnimMontage* AttackMontage = MeleeWeapon->GetLightComboAnimation(CurrentComboIndex);
    if (!AttackMontage) return false;

    return true;
}

bool UMeleeCombatComponent::TryLightAttack(AMeleeWeaponBase* MeleeWeapon, float AttackPower)
{
    if (!IsValid(MeleeWeapon)) return false;
    if (!IsValid(OwnerCharacter)) return false;

    UAnimMontage* AttackMontage = MeleeWeapon->GetLightComboAnimation(CurrentComboIndex);
    if (!AttackMontage) return false;

    float Duration = OwnerCharacter->PlayAnimMontage(AttackMontage);

    // 몽타주 재생 실패 시
    if (Duration <= 0.f) return false;


    // 트레이스 시작
    CurrentMeleeWeapon = MeleeWeapon;
    CurrentPower = AttackPower * MeleeWeapon->GetDamageRate();

    BeginAttackTrace();


    // 공격 사운드 재생
    if (USoundBase* AttackSound = MeleeWeapon->GetAttackSound())
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            AttackSound,
            OwnerCharacter->GetActorLocation()
            );
    }

    // 테스트용 애니메이션 종료를 알리는 함수
    // 몽타주가 끝나면 자동으로 호출됨
    if (bFinishedAttackForTest)
    {
        if (UWorld* World = GetWorld())
        {
            World->GetTimerManager().SetTimer(
                AttackFinishTimerHandle,
                this,
                &UMeleeCombatComponent::FinishAttack,
                Duration,
                false
                );
        }
    }

    return true;
}

bool UMeleeCombatComponent::TryHeavyAttack(AMeleeWeaponBase* MeleeWeapon, float AttackPower)
{
    if (!IsValid(MeleeWeapon)) return false;
    if (!IsValid(OwnerCharacter)) return false;

    UAnimMontage* AttackMontage = MeleeWeapon->GetLightComboAnimation(CurrentComboIndex);
    if (!IsValid(AttackMontage)) return false;

    float Duration = OwnerCharacter->PlayAnimMontage(AttackMontage);

    // 몽타주 재생 실패 시
    if (Duration <= 0.f) return false;


    // 트레이스 시작
    CurrentMeleeWeapon = MeleeWeapon;
    CurrentPower = AttackPower * MeleeWeapon->GetDamageRate();

    BeginAttackTrace();


    // 공격 사운드 재생
    if (USoundBase* AttackSound = MeleeWeapon->GetAttackSound())
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            AttackSound,
            OwnerCharacter->GetActorLocation()
            );
    }

    // 테스트용 애니메이션 종료를 알리는 함수
    // 몽타주가 끝나면 자동으로 호출됨
    if (bFinishedAttackForTest)
    {
        if (UWorld* World = GetWorld())
        {
            World->GetTimerManager().SetTimer(
                AttackFinishTimerHandle,
                this,
                &UMeleeCombatComponent::FinishAttack,
                Duration,
                false
                );
        }
    }


    return true;
}

void UMeleeCombatComponent::FinishAttack()
{
    EndAttackTrace();

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(AttackFinishTimerHandle);
    }

    CurrentComboIndex = 0;
    CurrentMeleeWeapon.Reset();
    CurrentPower = 0.f;

    OnAttackFinished.Broadcast();
}

void UMeleeCombatComponent::BeginAttackTrace()
{
    if (!CurrentMeleeWeapon.IsValid()) return;

    HitActors.Empty();

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            AttackTraceTimerHandle,
            this,
            &UMeleeCombatComponent::TraceAttack,
            AttackTraceInterval,
            true,
            0.f
            );
    }
}

void UMeleeCombatComponent::EndAttackTrace()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(AttackTraceTimerHandle);
    }

    HitActors.Empty();
}

void UMeleeCombatComponent::TraceAttack()
{
    AMeleeWeaponBase* Weapon = CurrentMeleeWeapon.Get();
    if (!IsValid(Weapon)) return;
    if (!IsValid(OwnerCharacter)) return;

    UStaticMeshComponent* WeaponMesh = Weapon->GetMesh();
    if (!IsValid(WeaponMesh)) return;

    FName StartSocketName = Weapon->GetTraceStartSocketName();
    FName EndSocketName = Weapon->GetTraceEndSocketName();

    // 손잡이, 무기 끝에 해당하는 소켓이 없다면 종료
    if (!WeaponMesh->DoesSocketExist(StartSocketName)) return;
    if (!WeaponMesh->DoesSocketExist(EndSocketName)) return;

    const FVector Start = WeaponMesh->GetSocketLocation(StartSocketName);
    const FVector End = WeaponMesh->GetSocketLocation(EndSocketName);

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(OwnerCharacter);
    QueryParams.AddIgnoredActor(Weapon);

    // 커스텀 채널 만들어서 적군이랑만 트레이스 하게 하는게 좋을듯
    TArray<FHitResult> HitResults;
    const bool bHit = GetWorld()->SweepMultiByChannel(
        HitResults,
        Start,
        End,
        FQuat::Identity,
        ECC_Pawn,
        FCollisionShape::MakeSphere(Weapon->GetTraceRadius()),
        QueryParams
        );

    // --- 디버그 캡슐 그리기 ---
    const float Radius = Weapon->GetTraceRadius();

    const FVector Center = (Start + End) * 0.5f;
    const FVector Direction = End - Start;
    const float HalfHeight = Direction.Size() * 0.5f + Radius;

    const FQuat Rotation = FRotationMatrix::MakeFromZ(Direction.GetSafeNormal()).ToQuat();

    DrawDebugCapsule(
        GetWorld(),
        Center,
        HalfHeight,
        Radius,
        Rotation,
        bHit ? FColor::Red : FColor::Green,
        false,
        AttackTraceInterval
    );
    // ------------------

    if (!bHit) return;

    for (const FHitResult& HitResult : HitResults)
    {
        AActor* HitActor = HitResult.GetActor();
        if (!IsValid(HitActor)) continue;

        AEnemyBase* HitEnemy = Cast<AEnemyBase>(HitActor);
        if (!IsValid(HitEnemy)) continue;

        if (HitActors.Contains(HitEnemy)) continue;

        HitActors.Add(HitEnemy);

        UGameplayStatics::ApplyDamage(
            HitActor,
            CurrentPower,
            OwnerCharacter->GetController(),
            OwnerCharacter,
            UDamageType::StaticClass()
            );
    }
}
