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
#include "Enemy/EnemyAIController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/DecalComponent.h"
#include "Enemy/BossHPUI.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

void ABossEnemy::BeginPlay()
{
    Super::BeginPlay();

    bCanBeStunned = false;

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

    TArray<FSoftObjectPath> AssetsToLoad;

    // 데칼 등록 확인
    if (!WarningDecalSoftClass.IsNull())
    {
        AssetsToLoad.Add(WarningDecalSoftClass.ToSoftObjectPath());
    }

    if (!ImpactDecalSoftClass.IsNull())
    {
        AssetsToLoad.Add(ImpactDecalSoftClass.ToSoftObjectPath());
    }

    FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
    // 데칼 로드
    if (AssetsToLoad.Num() > 0)
    {
        Streamable.RequestAsyncLoad(AssetsToLoad, FStreamableDelegate::CreateUObject(this, &ABossEnemy::OnDecalLoadCompleted));
    }
}

void ABossEnemy::OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != this && OtherActor->ActorHasTag(FName("Player")))
    {
        ACharacter* PlayerCharacter = Cast<ACharacter>(OtherActor);
        if (!PlayerCharacter)
            return;

        // 이미 때렸으면 그냥 종료
        if (HittedActors.Contains(OtherActor))
        {
            return;
        }

        // 때린놈으로 등록
        HittedActors.Add(OtherActor);


        //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("플레이어 피격 판정!"));
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

void ABossEnemy::DeactivateAttackCollision()
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

void ABossEnemy::OnEnemyHealthChanged(float ActualDamage, AController* EventInstigator)
{
    Super::OnEnemyHealthChanged(ActualDamage, EventInstigator);
    OnEnemyHit.Broadcast();
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
    if (!HitPlayer)
        return;

    UGameplayStatics::ApplyDamage(
        HitPlayer,
        GetAttackDamage(),
        GetController(),
        this,
        UDamageType::StaticClass()
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

void ABossEnemy::HandleHeavyAttackHit(ACharacter* HitPlayer)
{
    if (!HitPlayer)
        return;

    // 최종 데미지 산출
    float HeavyDamage = GetAttackDamage() * HeavyAttackDamageMultiplier;

    UGameplayStatics::ApplyDamage(
        HitPlayer,
        HeavyDamage,
        GetController(),
        this,
        UDamageType::StaticClass()
    );

    // 넉백 방향 계산
    FVector KnockbackDir = (HitPlayer->GetActorLocation() - GetActorLocation()).GetSafeNormal();

    // 3. '대각선 넉백'을 위해 Z축(위쪽) 방향을 더해주고 정규화
    KnockbackDir.Z = 0.6f;
    KnockbackDir.Normalize();

    // 넉백 힘을 곱해 날라가는 속도
    FVector LaunchVelocity = KnockbackDir * HeavyKnockbackForce;

    // 플레이어 날라감
    // 뒤의 옵션을 켜서 x,y,z속도 무시하고 덮어씀
    HitPlayer->LaunchCharacter(LaunchVelocity, true, true);

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

void ABossEnemy::HandleStompDirectHit(ACharacter* HitPlayer)
{
    if (!HitPlayer)
        return;

    // 스톰프 직접 타격 데미지
    float DirectDamage = GetAttackDamage();
    UGameplayStatics::ApplyDamage(
        HitPlayer,
        GetAttackDamage(),
        GetController(),
        this,
        UDamageType::StaticClass()
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

void ABossEnemy::PlayRoarAnimation()
{
    if (RoarMontage)
    {
        PlayAnimMontage(RoarMontage);
    }

    // UI생성
    ShowBossHPBar();
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
        SpawnStompWarningDecal();
        break;
    default:
        SectionName = FName("LightAttack");
        break;
    }

    PlayAnimMontage(AttackMontage, 1.0f, SectionName);
}

void ABossEnemy::ExecuteStomp()
{
    FVector BossLocation = GetActorLocation();

    TArray<FHitResult> Hits;
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(this);

    bool bHit = UKismetSystemLibrary::SphereTraceMulti(
        GetWorld(),
        BossLocation - FVector(0, 0, 150),
        BossLocation - FVector(0,0,150),
        StompRadius,
        UEngineTypes::ConvertToTraceType(ECC_Pawn),
        false,
        ActorsToIgnore,
        EDrawDebugTrace::ForDuration,
        Hits,
        true,
        FLinearColor::Green,
        FLinearColor::Red,
        2.0f
    );

    if (bHit)
    {
        TArray<AActor*> AoEHittedActors;

        for (const FHitResult& Hit : Hits)
        {
            AActor* HitActor = Hit.GetActor();
            ACharacter* HitChar = Cast<ACharacter>(HitActor);

            if (!HitChar || AoEHittedActors.Contains(HitActor))
                continue;

            // 플레이어 에게만 & 땅에있을때만 적용
            if (HitActor->ActorHasTag(FName("Player")) && !HitChar->GetCharacterMovement()->IsFalling())
            {
                // 혹시나할 중복적용을 방지
                AoEHittedActors.Add(HitActor);

                float StompDamage = GetAttackDamage() * StompAttackDamageMultiplier;

                // 데미지 처리
                UGameplayStatics::ApplyDamage(
                    HitActor,
                    StompDamage,
                    GetController(),
                    this,
                    UDamageType::StaticClass()
                );

                // 에어본
                HitChar->LaunchCharacter(FVector(0, 0, StompKnockupForce), false, true);

            }
        }
    }

    SpawnStompImpactDecal();
}

void ABossEnemy::ShowBossHPBar()
{
    // UI생성
    if (BossHPUIClass && !BossHPUIInstance)
    {
        BossHPUIInstance = GetWorld()->SpawnActor<ABossHPUI>(BossHPUIClass, FVector::ZeroVector, FRotator::ZeroRotator);
        if (BossHPUIInstance)
        {
            // 액터 내부의 위젯 생성 및 뷰포트 추가 함수 호출
            BossHPUIInstance->ShowBossHPBar();
        }
    }

    // 데이터 반영
    OnEnemyHit.Broadcast();
}

void ABossEnemy::HideBossHPBar()
{
    if (BossHPUIInstance)
    {
        BossHPUIInstance->HideBossHPBar(); // 위젯 제거
        BossHPUIInstance->Destroy();       // 액터 파괴
        BossHPUIInstance = nullptr;
    }
}

void ABossEnemy::UpdateBossHPBar()
{
    if (BossHPUIInstance)
    {
        BossHPUIInstance->UpdateBossHPBar();
    }
}

void ABossEnemy::Die()
{
    Super::Die();
}

void ABossEnemy::IMDead()
{
    // 콜리전 끄기
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 사망 애니메이션 재생
    if (DeathMontage)
    {
        PlayAnimMontage(DeathMontage, 0.5f, FName("Dead"));
    }

    if (AEnemyAIController* AIC = Cast<AEnemyAIController>(GetController()))
    {
        // 컨트롤러 사망 처리 호출
        AIC->SetDead();
    }
}

void ABossEnemy::ExecuteDestroy()
{
    HideBossHPBar();
    Super::ExecuteDestroy();
}

void ABossEnemy::OnDecalLoadCompleted()
{
    if (WarningDecalSoftClass.IsValid())
    {
        LoadedWarningDecalClass = WarningDecalSoftClass.Get();
    }

    if (ImpactDecalSoftClass.IsValid())
    {
        LoadedImpactDecalClass = ImpactDecalSoftClass.Get();
    }
}

void ABossEnemy::SpawnStompWarningDecal()
{
    if (LoadedWarningDecalClass && GetWorld())
    {
        // 보스 위치 바닥에 경고 장판 액터 소환
        // 삭제는 스스로될거임
        FVector SpawnLocation = GetActorLocation();
        SpawnLocation.Z -= 150.0f;

        GetWorld()->SpawnActor<AActor>(LoadedWarningDecalClass, SpawnLocation, FRotator::ZeroRotator);
    }
}

void ABossEnemy::SpawnStompImpactDecal()
{
    if (LoadedImpactDecalClass && GetWorld())
    {
        FVector SpawnLocation = GetActorLocation();
        SpawnLocation.Z -= 150.0f;

        GetWorld()->SpawnActor<AActor>(LoadedImpactDecalClass, SpawnLocation, FRotator::ZeroRotator);
    }
}
