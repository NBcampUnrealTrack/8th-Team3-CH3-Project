// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BossAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Enemy/EnemyBase.h"

ABossAIController::ABossAIController()
{
}

void ABossAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
}

void ABossAIController::UpdateCombatTactics()
{
}

void ABossAIController::StopEngaging()
{
}

void ABossAIController::OnHitDamage(APawn* Enemy)
{
}

void ABossAIController::SetDead()
{
}

void ABossAIController::OnAttackAnimationFinished()
{
}
