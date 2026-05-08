// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Enemy/EnemyAIController.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    HP = 100.f;
    AttackDamage = 10.f;
    bDead = false;
}

void AEnemyBase::OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 공격 처리
}

void AEnemyBase::ActivateAttackCollision()
{
    for (UPrimitiveComponent* Comp : WeaponCollisions)
    {
        if (Comp)
        {
            Comp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        }
    }
}

void AEnemyBase::DeactivateAttackCollision()
{
    for (UPrimitiveComponent* Comp : WeaponCollisions)
    {
        if (Comp)
        {
            Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
    }
}

void AEnemyBase::Die()
{
    // 중복 호출 방지용
    if (bDead) return;

    bDead = true;

    UE_LOG(LogTemp, Warning, TEXT("Enemy Died!"));

    // 바인딩한 이벤트들에게 Broadcast
    // 바인딩한 함수들을 처리하고 뒤의 Destroy 호출함
    OnEnemyDead.Broadcast(this);
    Destroy();
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

    TArray<UActorComponent*> FoundComps = GetComponentsByTag(UPrimitiveComponent::StaticClass(), FName("DamageBox"));

    for (UActorComponent* Comp : FoundComps)
    {
        UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(Comp);
        if (PrimitiveComp)
        {
            // 오버랩 함수 연결
            PrimitiveComp->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBase::OnWeaponOverlap);

            // 평소엔 꺼두기
            PrimitiveComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

            // 관리용 배열에 추가 (나중에 켜고 끌 때 사용)
            WeaponCollisions.Add(PrimitiveComp);
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
    if (!AttackMontage)
        return;

    // 캐릭터의 애니메이션 담당자(AnimInstance) 가져오기
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (!AnimInstance)
        return;

    // 공격방식 주사위 굴리기 (0 또는 1)
    int32 DiceRoll = FMath::RandRange(0, 1);

    // 굴린 결과에 따라 재생할 섹션 이름 결정
    FName SectionName;
    if (DiceRoll == 0)
    {
        SectionName = FName("Attack");  // 1타 전용 애니메이션
        GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("패턴: 단발 공격"));
    }
    else
    {
        SectionName = FName("Attack2"); // 2타 전용 연속 애니메이션
        GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("패턴: 2연타 콤보"));
    }

    // 결정된 섹션 이름으로 몽타주 재생
    PlayAnimMontage(AttackMontage, 1.0f, SectionName);
}

float AEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    HP -= FMath::Max(ActualDamage - Defance, 0.f);

    // AI 컨트롤러 가져오기
    AEnemyAIController* AIC = Cast<AEnemyAIController>(GetController());

    if (HP <= 0)
    {
        IMDead();
    }
    // 그로기 테스트용
    else if (HP <= (MaxHP * 0.3f))
    {
        IMGrogi();
    }

    return ActualDamage;
}
