// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/MinionEnemy.h"
#include "Kismet/GameplayStatics.h"

void AMinionEnemy::BeginPlay()
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
            PrimitiveComp->OnComponentBeginOverlap.AddDynamic(this, &AMinionEnemy::OnWeaponOverlap);
            // 평소엔 꺼두기
            PrimitiveComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            // 관리용 배열에 추가 (나중에 켜고 끌 때 사용)
            WeaponCollisions.Add(PrimitiveComp);
        }
    }
}

void AMinionEnemy::PlayAttack()
{
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
        //GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("패턴: 단발 공격"));
    }
    else
    {
        SectionName = FName("Attack2"); // 2타 전용 연속 애니메이션
        //GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("패턴: 2연타 콤보"));
    }

    // 결정된 섹션 이름으로 몽타주 재생
    PlayAnimMontage(AttackMontage, AttackSpeedRate, SectionName);
}

void AMinionEnemy::OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != this)
    {
        // 이미 때렸으면 그냥 종료
        if (HittedActors.Contains(OtherActor))
        {
            return;
        }

        // 때린놈으로 등록
        HittedActors.Add(OtherActor);

        // 맞은 액터가 Player 태그
        if (OtherActor->ActorHasTag(FName("Player")))
        {
            //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("플레이어 피격 판정!"));

            // 데미지 처리
            UGameplayStatics::ApplyDamage(
                OtherActor,                // 맞은 녀석 : 플레이어
                GetAttackDamage(),              // 데미지 수치 : TODO:상태컴포넌트 추가시 변경
                GetController(),           // 때린 녀석의 뇌 : AI 컨트롤러
                this,                      // 때린 녀석의 몸통 : 무기를 쥔 나 자신
                UDamageType::StaticClass() // 데미지 속성 : 기본 데미지
            );

            // 적중 사운드
            if (HitSound)
            {
                UGameplayStatics::PlaySoundAtLocation(
                    GetWorld(),
                    HitSound,
                    GetActorLocation()
                );
            }
        }
    }
}

void AMinionEnemy::ActivateAttackCollision(FName WeaponTag)
{
    HittedActors.Empty();

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
    OnAttackCollisionStarted.Broadcast();
}

void AMinionEnemy::DeactivateAttackCollision()
{
    HittedActors.Empty();

    for (UPrimitiveComponent* Comp : WeaponCollisions)
    {
        if (Comp)
        {
            Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
    }
    OnAttackCollisionEnded.Broadcast();
}
