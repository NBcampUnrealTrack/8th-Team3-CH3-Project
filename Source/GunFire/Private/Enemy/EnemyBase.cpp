// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Enemy/EnemyAIController.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "StatComponent.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("StatComponent"));
    bDead = false;
    AttackSpeedRate = 1.0f;
}

void AEnemyBase::BeginPlay()
{
    Super::BeginPlay();

    //TArray<UActorComponent*> FoundComps = GetComponentsByTag(UPrimitiveComponent::StaticClass(), FName("DamageBox"));
    //
    //for (UActorComponent* Comp : FoundComps)
    //{
    //    UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(Comp);
    //    if (PrimitiveComp)
    //    {
    //        // 오버랩 함수 연결
    //        PrimitiveComp->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBase::OnWeaponOverlap);
    //
    //        // 평소엔 꺼두기
    //        PrimitiveComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    //
    //        // 관리용 배열에 추가 (나중에 켜고 끌 때 사용)
    //        WeaponCollisions.Add(PrimitiveComp);
    //    }
    //}

    if (StatComponent)
    {
        // 죽을 시 OnEnemyDeath 함수 실행
        StatComponent->OnDead.AddDynamic(this, &AEnemyBase::OnEnemyDeath);
        // 체력바뀔 시 OnEnemyHealthChanged 함수 실행
        StatComponent->OnDamaged.AddDynamic(this, &AEnemyBase::OnEnemyHealthChanged);
    }

    // 사망 애니메이션 호출 테스트 용도
    // 테스트 끝나면 삭제 필요
    //FTimerHandle DebugDeathTimer;
    //GetWorld()->GetTimerManager().SetTimer(DebugDeathTimer, this, &AEnemyBase::Die, 10.0f, false);

    // 테스트용
    if (bIsDebugTestTarget)
    {
        FTimerHandle DebugHitTimer;
        GetWorld()->GetTimerManager().SetTimer(DebugHitTimer, FTimerDelegate::CreateLambda([this]()
            {
                APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
                AController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

                if (PlayerPawn && PlayerController && !bDead)
                {
                    UGameplayStatics::ApplyDamage(this, 30.0f, PlayerController, PlayerPawn, nullptr);
                }
            }), 3.0f, true);
    }
}

void AEnemyBase::PlayHitReaction()
{
    if (bDead || !HitMontage)
        return;

    // 현재 재생 중인 몽타주 등이 있다면 중단
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance)
    {
        AnimInstance->Montage_Play(HitMontage);
    }
}

void AEnemyBase::OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 공격 처리
}

void AEnemyBase::ActivateAttackCollision(FName WeaponTag)
{
    //for (UPrimitiveComponent* Comp : WeaponCollisions)
    //{
    //    if (Comp)
    //    {
    //        Comp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    //    }
    //}
}

void AEnemyBase::DeactivateAttackCollision()
{
    //for (UPrimitiveComponent* Comp : WeaponCollisions)
    //{
    //    if (Comp)
    //    {
    //        Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    //    }
    //}
}

void AEnemyBase::OnDeathAnimationFinished()
{
    // 래그돌로 충돌되도록 변경
    GetMesh()->SetCollisionProfileName(FName("Ragdoll"));

    // 중력에 따라 래그돌 되도록 변경
    GetMesh()->SetSimulatePhysics(true);
}

void AEnemyBase::Die()
{
    // 중복 호출 방지용
    if (bDead) return;

    bDead = true;

    UE_LOG(LogTemp, Warning, TEXT("Enemy Died!"));

    // 사망시 처리할 요소들
    IMDead();

    // 바인딩한 이벤트들에게 Broadcast
    // 바인딩한 함수들을 처리하고 뒤의 Destroy 호출함
    OnEnemyDead.Broadcast(this);

    // 일정 시간뒤에 삭제
    GetWorld()->GetTimerManager().SetTimer(DeathTimerHandle, this, &AEnemyBase::ExecuteDestroy, 10.0f, false);
}

void AEnemyBase::ExecuteDestroy()
{
    Destroy();
}

void AEnemyBase::OnEnemyDeath(AController* InstigatorController)
{
    Die();
}

void AEnemyBase::OnEnemyHealthChanged(float ActualDamage, AController* EventInstigator)
{
    // 공격당햇음을 알림
    if (EventInstigator)
    {
        // 공격자 폰을 가져옴
        APawn* AttackerPawn = EventInstigator->GetPawn();
        if (AttackerPawn)
        {
            if (AEnemyAIController* AIC = Cast<AEnemyAIController>(GetController()))
            {
                // AI 컨트롤러에게 피격당햇음을 알림
                AIC->OnHitDamage(AttackerPawn);
            }
        }
    }
}

void AEnemyBase::SetWalkSpeed(float _speed)
{
    UCharacterMovementComponent* chacomp = GetCharacterMovement();
    chacomp->MaxWalkSpeed = _speed;
}

void AEnemyBase::IMDead()
{
    // 애니메이션 중단 및 사망 애니메이션/몽타주 재생
    //StopAnimMontage();

    // 콜리전 끄기
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 사망 애니메이션 재생
    if (DeathMontage)
    {
        // 1부터 5까지 랜덤 주사위 굴리기
        int32 DiceRoll = FMath::RandRange(1, 5);

        // Death1 ~ Death5중 무작위로 사망 매니메이션
        FName SectionName = FName(*FString::Printf(TEXT("Death%d"), DiceRoll));
        PlayAnimMontage(DeathMontage, 1.0f, SectionName);
    }

    if (AEnemyAIController* AIC = Cast<AEnemyAIController>(GetController()))
    {
        // 컨트롤러 사망 처리 호출
        AIC->SetDead();
    }
}

void AEnemyBase::IMGrogi()
{
    //StopAnimMontage();

    AEnemyAIController* AIC = Cast<AEnemyAIController>(GetController());
    if (AIC)
    {
        AIC->ForceResetAttack(); // 공격 강제 해제
    }
}

void AEnemyBase::PlayAttack()
{
    //// 에디터에서 AttackMontage를 할당했다면 실행됨
    //if (AttackMontage)
    //{
    //    // 몽타주 재생 함수 (ACharacter 기본 함수)
    //    PlayAnimMontage(AttackMontage);
    //}

    // 몽타주가 세팅되어 있는지 확인
    //if (!AttackMontage)
    //    return;

    //// 캐릭터의 애니메이션 담당자(AnimInstance) 가져오기
    //UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    //if (!AnimInstance)
    //    return;
    //
    //// 공격방식 주사위 굴리기 (0 또는 1)
    //int32 DiceRoll = FMath::RandRange(0, 1);
    //
    //// 굴린 결과에 따라 재생할 섹션 이름 결정
    //FName SectionName;
    //if (DiceRoll == 0)
    //{
    //    SectionName = FName("Attack");  // 1타 전용 애니메이션
    //    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("패턴: 단발 공격"));
    //}
    //else
    //{
    //    SectionName = FName("Attack2"); // 2타 전용 연속 애니메이션
    //    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("패턴: 2연타 콤보"));
    //}
    //
    //// 결정된 섹션 이름으로 몽타주 재생
    //PlayAnimMontage(AttackMontage, 1.0f, SectionName);
}

//float AEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
//{
//    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
//
//    HP -= FMath::Max(ActualDamage - Defance, 0.f);
//
//    // AI 컨트롤러 가져오기
//    AEnemyAIController* AIC = Cast<AEnemyAIController>(GetController());
//
//    if (HP <= 0)
//    {
//        Die();
//    }
//    // 그로기 테스트용
//    else if (HP <= (MaxHP * 0.3f))
//    {
//        IMGrogi();
//    }
//
//    return ActualDamage;
//}

float AEnemyBase::GetHP() const
{
    return StatComponent ? StatComponent->GetCurrentHealth() : 0.f;
}

float AEnemyBase::GetMaxHP() const
{
    return StatComponent ? StatComponent->GetMaxHealth() : 0.f;
}

float AEnemyBase::GetAttackDamage() const
{
    return StatComponent ? StatComponent->GetAttackPower() : 0.f;
}

bool AEnemyBase::bIsDead() const
{
    return bDead;
}
