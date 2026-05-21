// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BossHPUI.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/BossEnemy.h"

// Sets default values
ABossHPUI::ABossHPUI()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ABossHPUI::BeginPlay()
{
	Super::BeginPlay();

    TargetBoss = Cast<ABossEnemy>(UGameplayStatics::GetActorOfClass(GetWorld(), ABossEnemy::StaticClass()));

    // 체력갱신은 보스의 체력이 달경우
    if (TargetBoss)
    {
        TargetBoss->OnEnemyHit.AddDynamic(this, &ABossHPUI::UpdateBossHPBar);
    }
}

void ABossHPUI::ShowBossHPBar()
{
    if (!BossHPWidgetInstance && BossHPWidgetClass)
    {
        BossHPWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), BossHPWidgetClass);
        if (BossHPWidgetInstance)
        {
            BossHPWidgetInstance->AddToViewport();
        }
    }

    // 켜자마자 현재 체력으로 초기화
    UpdateBossHPBar();

}

void ABossHPUI::HideBossHPBar()
{
    if (BossHPWidgetInstance)
    {
        BossHPWidgetInstance->RemoveFromParent();
        BossHPWidgetInstance = nullptr;
    }
}

void ABossHPUI::UpdateBossHPBar()
{
    // 타겟 보스가 없거나 위젯이 안 띄워져 있으면 무시
    if (!TargetBoss || !BossHPWidgetInstance)
        return;

    if (UTextBlock* BossName = Cast<UTextBlock>(BossHPWidgetInstance->GetWidgetFromName(TEXT("BossName"))))
    {
        BossName->SetText(TargetBoss->EnemyName);
    }

    if (UProgressBar* BossHPBar = Cast<UProgressBar>(BossHPWidgetInstance->GetWidgetFromName(TEXT("BossHPBar"))))
    {
        // 타겟 보스의 체력으로 세팅
        float MaxHP = TargetBoss->GetMaxHP();
        float HPPercent = (MaxHP > 0.f) ? (TargetBoss->GetHP() / MaxHP) : 0.0f;
        BossHPBar->SetPercent(HPPercent);
    }
}
