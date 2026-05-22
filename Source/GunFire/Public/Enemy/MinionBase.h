// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBase.h"
#include "MinionBase.generated.h"

/**
 * 
 */
UCLASS()
class GUNFIRE_API AMinionBase : public AEnemyBase
{
	GENERATED_BODY()

public:
    AMinionBase();

protected:
    // HP용 위젯
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    class UWidgetComponent* HPBarWidgetComp;

    // 일정 시간 뒤에 HP바를 끄기 위한 타이머
    FTimerHandle HPBarTimerHandle;

    // HP바가 켜져 있을 유지 시간
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    float HPBarVisibleDuration = 3.0f;

    // HP관련함수
    virtual void ShowHPBar();
    virtual void HideHPBar();

    virtual void OnEnemyHealthChanged(float ActualDamage, AController* EventInstigator) override;

    // 상속받은 minion들이 구현해야할 함수
    virtual void BeginPlay() override { Super::BeginPlay(); };

public:
    virtual void PlayAttack() override {};
    virtual void OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override {};
    virtual void ActivateAttackCollision(FName WeaponTag) override {};
    virtual void DeactivateAttackCollision() override {};
};
