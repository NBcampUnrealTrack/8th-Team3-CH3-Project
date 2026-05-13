// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BossEnemy.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Enemy/EnemyBase.h"
#include "TimerManager.h"

void ABossEnemy::BeginPlay()
{
}

void ABossEnemy::PlayAttack()
{
}

void ABossEnemy::ActivateAttackCollision(FName WeaponTag)
{
}

void ABossEnemy::DeactivateAttackCollision()
{
}

void ABossEnemy::OnEnemyHealthChanged(float ActualDamage, AController* EventInstigator)
{
}

void ABossEnemy::Die()
{
}

void ABossEnemy::IMDead()
{
}
