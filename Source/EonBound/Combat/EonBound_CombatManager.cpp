// Fill out your copyright notice in the Description page of Project Settings.


#include "EonBound_CombatManager.h"

UEonBound_CombatManager::UEonBound_CombatManager()
{
}

void UEonBound_CombatManager::StartCombat()
{	
	
}

void UEonBound_CombatManager::RequestPlayerBasicAttack(AActor* Target)
{
}

void UEonBound_CombatManager::NotifyActionFinished()
{
}

void UEonBound_CombatManager::CheckCombatEnd()
{
}

EEonBound_CombatState UEonBound_CombatManager::GetCombatState() const
{
	return CombatState;
}

void UEonBound_CombatManager::SetCombatState(EEonBound_CombatState NewState)
{
}

void UEonBound_CombatManager::SelectNextCombatant()
{
}

void UEonBound_CombatManager::StartEnemyTurn()
{
}

bool UEonBound_CombatManager::IsCombatantAlive(AActor* Combatant) const
{
	if (!Combatant)
	{
		return false;
	}
	return !Combatant->IsPendingKillPending();
}
