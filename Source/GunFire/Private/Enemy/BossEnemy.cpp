// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BossEnemy.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Enemy/EnemyBase.h"
#include "AIController.h"
#include "TimerManager.h"

void ABossEnemy::BeginPlay()
{
    Super::BeginPlay();

    // 공격충돌체 등록
    TArray<UActorComponent*> FoundComps = GetComponentsByTag(UPrimitiveComponent::StaticClass(), FName("DamageBox"));
    for (UActorComponent* Comp : FoundComps)
    {
        UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(Comp);
        if (PrimitiveComp)
        {
            // 오버랩 함수 연결
            PrimitiveComp->OnComponentBeginOverlap.AddDynamic(this, &ABossEnemy::OnWeaponOverlap);
            // 평소엔 꺼두기
            PrimitiveComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            // 관리용 배열에 추가 (나중에 켜고 끌 때 사용)
            WeaponCollisions.Add(PrimitiveComp);
        }
    }
}

void ABossEnemy::OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != this && OtherActor->ActorHasTag(FName("Player")))
    {
        ACharacter* PlayerCharacter = Cast<ACharacter>(OtherActor);
        if (!PlayerCharacter) return;

        // 공격 패턴에 따라처리
        switch (CurrentAttackPattern)
        {
        case 0:
            HandleLightAttackHit(PlayerCharacter);
            break;
        case 1:
            HandleHeavyAttackHit(PlayerCharacter);
            break;
        case 2:
            HandleStompDirectHit(PlayerCharacter);
            break;
        default:
            break;
        }
    }
}

void ABossEnemy::ActivateAttackCollision(FName WeaponTag)
{
    for (UPrimitiveComponent* Comp : WeaponCollisions)
    {
        if (Comp)
        {
            if (Comp->ComponentTags.Contains(WeaponTag))
            {
                Comp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

                // 확인용 디버그 로그
                //GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Cyan, FString::Printf(TEXT("%s 손 판정 ON!"), *WeaponTag.ToString()));
            }
        }
    }
}

void ABossEnemy::DeactivateAttackCollision()
{
    for (UPrimitiveComponent* Comp : WeaponCollisions)
    {
        if (Comp)
        {
            Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
    }
}

void ABossEnemy::OnEnemyHealthChanged(float ActualDamage, AController* EventInstigator)
{
    Super::OnEnemyHealthChanged(ActualDamage, EventInstigator);
}

void ABossEnemy::PlayAttack()
{
    AController* MyController = GetController();
    if (!MyController)
        return;

    if (AAIController* AIC = Cast<AAIController>(MyController))
    {
        if (UBlackboardComponent* BBComp = AIC->GetBlackboardComponent())
        {
            // 블랙보드의 데이터를 가져와 어떤공격할지 출력
            int32 PatternIndex = BBComp->GetValueAsInt(FName("AttackPattern"));
            PlayBossAttack(PatternIndex);
        }
    }
}

void ABossEnemy::HandleLightAttackHit(ACharacter* HitPlayer)
{
    UGameplayStatics::ApplyDamage(HitPlayer, GetAttackDamage(), GetController(), this, UDamageType::StaticClass());
}

void ABossEnemy::HandleHeavyAttackHit(ACharacter* HitPlayer)
{
}

void ABossEnemy::HandleStompDirectHit(ACharacter* HitPlayer)
{
}

void ABossEnemy::PlayRoarAnimation()
{
    if (RoarMontage)
    {
        PlayAnimMontage(RoarMontage);
    }
}

void ABossEnemy::PlayBossAttack(int32 PatternIndex)
{
    if (!AttackMontage)
        return;

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (!AnimInstance)
        return;

    // 현재 공격 패턴 상태를 갱신하여 Overlap 함수가 참조할 수 있게 함
    CurrentAttackPattern = PatternIndex;

    FName SectionName;
    switch (PatternIndex)
    {
    case 0:
        SectionName = FName("LightAttack");
        break;
    case 1:
        SectionName = FName("HeavyAttack");
        break;
    case 2:
        SectionName = FName("StompAttack");
        break;
    default:
        SectionName = FName("LightAttack");
        break;
    }

    // TODO : 섹션네임으로 바꾸기
    PlayAnimMontage(AttackMontage, 1.0f, FName("LightAttack"));
}

void ABossEnemy::ExecuteStomp()
{
}

void ABossEnemy::Die()
{
    Super::Die();
}

void ABossEnemy::IMDead()
{
    Super::IMDead();
}
