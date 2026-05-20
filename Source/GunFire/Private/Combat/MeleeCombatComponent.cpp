#include "Combat/MeleeCombatComponent.h"

#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon/MeleeWeaponBase.h"
#include "DrawDebugHelpers.h"
#include "PlayerCharacter.h"
#include "Enemy/EnemyBase.h"

UMeleeCombatComponent::UMeleeCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

    OwnerCharacter = nullptr;
    CurrentAttackType = EMeleeAttackType::Light;
    CurrentComboIndex = 0;

    bCanComboInput = false;
    bComboTriggered = false;
    bAttackInProgress = false;

    CurrentPower = 0.f;
    CurrentAttackRotation = FRotator::ZeroRotator;
    AttackTraceInterval = 0.01f;
}

void UMeleeCombatComponent::BeginPlay()
{
    Super::BeginPlay();

    AActor* Owner = GetOwner();
    if (!IsValid(Owner)) return;

    OwnerCharacter = Cast<ACharacter>(Owner);
}

bool UMeleeCombatComponent::CanStartAttack(AMeleeWeaponBase* MeleeWeapon, EMeleeAttackType AttackType,
                                           int32 ComboIndex) const
{
    if (!IsValid(MeleeWeapon)) return false;
    if (!IsValid(OwnerCharacter)) return false;

    UAnimMontage* AttackMontage = GetAttackAnimMontage(MeleeWeapon, AttackType);
    if (!AttackMontage) return false;

    // 섹션 이름이 존재하는지, 섹션 이름이 해당 몽타주에 제대로 있는지 확인
    FName SectionName = GetComboSectionName(MeleeWeapon, AttackType, ComboIndex);
    if (SectionName.IsNone() || AttackMontage->GetSectionIndex(SectionName) == INDEX_NONE) return false;

    return true;
}

bool UMeleeCombatComponent::CanAcceptAttackInput(AMeleeWeaponBase* MeleeWeapon, EMeleeAttackType AttackType) const
{
    // 공격중이 아니라면 첫번째 공격으로 판단
    if (!IsAttackInProgress())
    {
        return CanStartAttack(MeleeWeapon, AttackType, 0);
    }

    // 공격 중간에 무기가 사라졌다면 종료
    if (!IsValidCurrentWeapon()) return false;

    // 무기가 중간에 바뀌었는지 확인
    if (CurrentMeleeWeapon.Get() != MeleeWeapon) return false;

    // 약공격 <-> 강공격 전환 금지
    if (CurrentAttackType != AttackType) return false;

    // 콤보를 이어갈 수 없거나 이미 다음 콤보 입력이 들어왔다면 false
    if (!bCanComboInput || bComboTriggered) return false;

    return CanStartAttack(MeleeWeapon, AttackType, CurrentComboIndex + 1);
}

bool UMeleeCombatComponent::IsAttackInProgress() const
{
    return bAttackInProgress;
}

bool UMeleeCombatComponent::IsValidCurrentWeapon() const
{
    return CurrentMeleeWeapon.IsValid();
}

UAnimMontage* UMeleeCombatComponent::GetAttackAnimMontage(AMeleeWeaponBase* MeleeWeapon, EMeleeAttackType AttackType) const
{
    if (!IsValid(MeleeWeapon)) return nullptr;

    return AttackType == EMeleeAttackType::Light
        ? MeleeWeapon->GetLightComboAnimMontage()
        : MeleeWeapon->GetHeavyComboAnimMontage();
}

bool UMeleeCombatComponent::TryLightAttack(AMeleeWeaponBase* MeleeWeapon, float AttackPower)
{
    return TryAttack(MeleeWeapon, AttackPower, EMeleeAttackType::Light);
}

bool UMeleeCombatComponent::TryHeavyAttack(AMeleeWeaponBase* MeleeWeapon, float AttackPower)
{
    return TryAttack(MeleeWeapon, AttackPower, EMeleeAttackType::Heavy);
}

void UMeleeCombatComponent::FinishAttack()
{
    if (!IsAttackInProgress()) return;

    EndAttackTrace();

    ResetAttackState();

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
            &UMeleeCombatComponent::DoHitTrace,
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
}

void UMeleeCombatComponent::OpenComboInput()
{
    if (!IsAttackInProgress()) return;
    if (!IsValidCurrentWeapon()) return;

    bCanComboInput = true;
    bComboTriggered = false;
}

void UMeleeCombatComponent::CloseComboInput()
{
    bCanComboInput = false;
}

void UMeleeCombatComponent::UpdateAttackDirection()
{
    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OwnerCharacter);
    if (!IsValid(PlayerCharacter)) return;

    if (!PlayerCharacter->HasMovementInput()) return;

    CurrentAttackRotation = PlayerCharacter->GetAttackInputRotation();
}

FRotator UMeleeCombatComponent::GetCurrentAttackRotation() const
{
    return CurrentAttackRotation;
}

bool UMeleeCombatComponent::TryAttack(AMeleeWeaponBase* MeleeWeapon, float AttackPower, EMeleeAttackType AttackType)
{
    if (IsAttackInProgress())
    {
        return TryChainAttack(MeleeWeapon, AttackPower, AttackType);
    }

    return StartAttack(MeleeWeapon, AttackPower, AttackType);
}

bool UMeleeCombatComponent::StartAttack(AMeleeWeaponBase* MeleeWeapon, float AttackPower, EMeleeAttackType AttackType)
{
    if (!CanStartAttack(MeleeWeapon, AttackType, 0)) return false;

    UAnimMontage* AttackMontage = GetAttackAnimMontage(MeleeWeapon, AttackType);
    if (!AttackMontage)
    {
        UE_LOG(LogTemp, Warning, TEXT("공격 애니메이션 몽타주가 없습니다!!"));
        return false;
    }

    FName SectionName = GetComboSectionName(MeleeWeapon, AttackType, 0);

    float Duration = OwnerCharacter->PlayAnimMontage(AttackMontage, 1.f, SectionName);
    if (Duration <= 0.f)
    {
        UE_LOG(LogTemp, Warning, TEXT("애니메이션 몽타주를 실행에 실패했습니다!!"));
        return false;
    }

    UAnimInstance* AnimInstance = OwnerCharacter->GetMesh() ? OwnerCharacter->GetMesh()->GetAnimInstance() : nullptr;
    if (!AnimInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("메시 읽기에 실패했거나, 애니메이션 인스턴스를 가져오는데 실패했습니다!!"));
        return false;
    }

    CurrentMeleeWeapon = MeleeWeapon;
    CurrentPower = AttackPower * MeleeWeapon->GetDamageRate();
    CurrentAttackType = AttackType;
    CurrentComboIndex = 0;
    bCanComboInput = false;
    bComboTriggered = false;
    bAttackInProgress = true;

    // 애니메이션 몽타주가 끝날 때 호출할 함수 바인딩
    FOnMontageEnded OnAttackMontageEnded;
    OnAttackMontageEnded.BindUObject(this, &UMeleeCombatComponent::HandleAttackMontageEnded);
    AnimInstance->Montage_SetEndDelegate(OnAttackMontageEnded, AttackMontage);

    PlayAttackSound(MeleeWeapon);

    return true;
}

bool UMeleeCombatComponent::TryChainAttack(AMeleeWeaponBase* MeleeWeapon, float AttackPower,
    EMeleeAttackType AttackType)
{
    if (!CanAcceptAttackInput(MeleeWeapon, AttackType)) return false;

    UAnimMontage* AttackMontage = GetAttackAnimMontage(MeleeWeapon, AttackType);
    if (!AttackMontage)
    {
        UE_LOG(LogTemp, Warning, TEXT("공격 애니메이션 몽타주가 없습니다!!"));
        return false;
    }

    UAnimInstance* AnimInstance = OwnerCharacter->GetMesh() ? OwnerCharacter->GetMesh()->GetAnimInstance() : nullptr;
    if (!AnimInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("메시 읽기에 실패했거나, 애니메이션 인스턴스를 가져오는데 실패했습니다!!"));
        return false;
    }

    int32 NextComboIndex = CurrentComboIndex + 1;
    FName NextSectionName = GetComboSectionName(MeleeWeapon, AttackType, NextComboIndex);
    if (NextSectionName.IsNone())
    {
        UE_LOG(LogTemp, Warning, TEXT("콤보에 해당하는 섹션 이름이 없습니다!!"));
        return false;
    }

    // 입력은 들어왔지만 몽타주 재생이 이미 끝났을 경우 실패 반환
    if (!AnimInstance->Montage_IsPlaying(AttackMontage))
    {
        UE_LOG(LogTemp, Warning, TEXT("이미 몽타주 재생이 끝났습니다!!"));
        return false;
    }

    // 현재 섹션을 건너뛸 경우 NotifyState End가 실행되지 않을 수 있으므로 직접 처리해야 함
    EndAttackTrace();
    CloseComboInput();
    MeleeWeapon->StopTrailEffect();

    // 다음 동작의 섹션으로 건너뛰기
    AnimInstance->Montage_JumpToSection(NextSectionName, AttackMontage);
    // 제대로 건너 뛰었는지 확인
    if (AnimInstance->Montage_GetCurrentSection(AttackMontage) != NextSectionName) return false;

    CurrentComboIndex = NextComboIndex;
    CurrentPower = AttackPower * MeleeWeapon->GetDamageRate();
    bComboTriggered = true;
    bCanComboInput = false;

    PlayAttackSound(MeleeWeapon);

    return true;
}

FName UMeleeCombatComponent::GetComboSectionName(AMeleeWeaponBase* MeleeWeapon, EMeleeAttackType AttackType,
    int32 ComboIndex) const
{
    if (!IsValid(MeleeWeapon)) return NAME_None;

    return AttackType == EMeleeAttackType::Light
        ? MeleeWeapon->GetLightComboSectionName(ComboIndex)
        : MeleeWeapon->GetHeavyComboSectionName(ComboIndex);
}

void UMeleeCombatComponent::ResetAttackState()
{
    CurrentAttackType = EMeleeAttackType::Light;
    CurrentComboIndex = 0;
    bCanComboInput = false;
    bComboTriggered = false;
    bAttackInProgress = false;
    CurrentMeleeWeapon.Reset();
    CurrentPower = 0.f;
    HitActors.Empty();
}

void UMeleeCombatComponent::PlayAttackSound(AMeleeWeaponBase* MeleeWeapon) const
{
    if (!IsValid(MeleeWeapon) || !IsValid(OwnerCharacter)) return;

    if (USoundBase* AttackSound = MeleeWeapon->GetAttackSound())
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            AttackSound,
            OwnerCharacter->GetActorLocation()
            );
    }
}

void UMeleeCombatComponent::HandleAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (!IsAttackInProgress()) return;

    // 공격중에 무기가 사라지면 공격 흐름만 정리함
    if (!IsValidCurrentWeapon())
    {
        FinishAttack();
        return;
    }

    // 종료 이벤트가 공격 몽타주의 종료 이벤트인지 확인
    // 피격으로 끊기거나 할 수 있으니 맞는지 체크
    AMeleeWeaponBase* MeleeWeapon = CurrentMeleeWeapon.Get();
    if (Montage != GetAttackAnimMontage(MeleeWeapon, CurrentAttackType)) return;

    FinishAttack();
}

void UMeleeCombatComponent::DoHitTrace()
{
    AMeleeWeaponBase* Weapon = CurrentMeleeWeapon.Get();
    if (!IsValid(Weapon)) return;
    if (!IsAttackInProgress()) return;
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
#if ENABLE_DRAW_DEBUG
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
#endif
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
