// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/MinionBase.h"
#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"

AMinionBase::AMinionBase()
    :AEnemyBase()
{
    // HP위젯
    HPBarWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarWidgetComp"));
    HPBarWidgetComp->SetupAttachment(RootComponent);

    HPBarWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
    HPBarWidgetComp->SetVisibility(false);
}

void AMinionBase::ShowHPBar()
{
    if (!HPBarWidgetComp)
        return;

    // 숨겨진 위젯 컴포넌트를 즉시 활성화
    HPBarWidgetComp->SetVisibility(true);

    // 위젯 오브젝트를 가져와서 갱신
    UUserWidget* UserWidget = HPBarWidgetComp->GetUserWidgetObject();
    if (UserWidget)
    {
        // HP비율 계산(0이하면 0)
        float MaxHP = GetMaxHP();
        float CurrentHPPercent = (MaxHP > 0.f) ? (GetHP() / MaxHP) : 0.f;

        // HPBar 를 조정
        if (UProgressBar* HPBar = Cast<UProgressBar>(HPBarWidgetComp->GetWidget()->GetWidgetFromName(FName("HPBar"))))
        {
            float HPPercent = (GetHP() > 0) ? ((float)GetHP() / (float)GetMaxHP()) : 0.0f;
            HPBar->SetPercent(HPPercent);
        }
    }

    // 타이머 설정(맴버번수로 설정된 HideHPBar호출)
    if (GetWorld())
    {
        // 연타로 맞을 경우를 대비해 기존 타이머 초기화
        GetWorld()->GetTimerManager().ClearTimer(HPBarTimerHandle);

        // 3초(3.0f) 뒤에 HideHPBar를 호출하도록 새 타이머 설정
        GetWorld()->GetTimerManager().SetTimer(
            HPBarTimerHandle,
            this,
            &AMinionBase::HideHPBar,
            3.0f,
            false
        );
    }
}

void AMinionBase::HideHPBar()
{
    // 위젯 숨기기
    if (HPBarWidgetComp)
    {
        HPBarWidgetComp->SetVisibility(false);
    }
}

void AMinionBase::OnEnemyHealthChanged(float ActualDamage, AController* EventInstigator)
{
    Super::OnEnemyHealthChanged(ActualDamage, EventInstigator);

    ShowHPBar();
}
