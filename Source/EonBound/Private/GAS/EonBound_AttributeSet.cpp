// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/EonBound_AttributeSet.h"

#include "Net/UnrealNetwork.h"

UEonBound_AttributeSet::UEonBound_AttributeSet()
{
}

void UEonBound_AttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UEonBound_AttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UEonBound_AttributeSet, HealthMax, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UEonBound_AttributeSet, Attack, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UEonBound_AttributeSet, Defense, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UEonBound_AttributeSet, Speed, COND_None, REPNOTIFY_Always);
}

void UEonBound_AttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEonBound_AttributeSet, Health, OldHealth);
}

void UEonBound_AttributeSet::OnRep_HealthMax(const FGameplayAttributeData& OldHealthMax)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEonBound_AttributeSet, Health, OldHealthMax);
}

void UEonBound_AttributeSet::OnRep_Attack(const FGameplayAttributeData& OldAttack)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEonBound_AttributeSet, Attack, OldAttack);
}

void UEonBound_AttributeSet::OnRep_Defense(const FGameplayAttributeData& OldDefense)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEonBound_AttributeSet, Defense, OldDefense);
}

void UEonBound_AttributeSet::OnRep_Speed(const FGameplayAttributeData& OldSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEonBound_AttributeSet, Speed, OldSpeed);
}