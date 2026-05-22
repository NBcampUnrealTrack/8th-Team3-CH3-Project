// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossHPUI.generated.h"

class UUserWidget;

UCLASS()
class GUNFIRE_API ABossHPUI : public AActor
{
	GENERATED_BODY()
	
public:	
	ABossHPUI();

protected:
    virtual void BeginPlay() override;

public:
    // UI 생성 및 출력
    UFUNCTION(BlueprintCallable, Category = "Boss UI")
    void ShowBossHPBar();

    // UI 제거
    UFUNCTION(BlueprintCallable, Category = "Boss UI")
    void HideBossHPBar();

    // 체력 갱신
    UFUNCTION(BlueprintCallable, Category = "Boss UI")
    void UpdateBossHPBar();

protected:
    // 위젯 세팅
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> BossHPWidgetClass;

    UPROPERTY()
    UUserWidget* BossHPWidgetInstance;

    // 체력을 읽어올 타겟 보스 캐싱용
    UPROPERTY()
    class ABossEnemy* TargetBoss;

};
