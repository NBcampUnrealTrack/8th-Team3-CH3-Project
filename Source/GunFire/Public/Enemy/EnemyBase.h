// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

UCLASS()
class GUNFIRE_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyBase();

    UFUNCTION()
    virtual void OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual void ActivateAttackCollision();
    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual void DeactivateAttackCollision();

protected:
    UPROPERTY()
    TArray<UPrimitiveComponent*> WeaponCollisions;

    UPROPERTY(EditAnywhere, Category = "Stats")
    UAnimMontage* AttackMontage;

    UPROPERTY(EditAnywhere, Category = "Stats")
    float HP;
    UPROPERTY(EditAnywhere, Category = "Stats")
    float MaxHP;

    UPROPERTY(EditAnywhere, Category = "Stats")
    float Defance;

    UPROPERTY(EditAnywhere, Category = "Stats")
    float AttackDamage;


	virtual void BeginPlay() override;

public:
    // 이동속도 변경
    void SetWalkSpeed(float _speed);

    // 사망상태
    virtual void IMDead();

    //그로기 상태
    virtual void IMGrogi();

    // 공격애니메이션 재생
    // 부모에 구현된 코드는 예시코드임
    UFUNCTION(BlueprintCallable, Category = "AI|Combat")
    virtual void PlayAttack();

    // 피해 처리
    UFUNCTION(BlueprintCallable, Category = "Health")
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


};
