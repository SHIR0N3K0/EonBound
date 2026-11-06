// Fill out your copyright notice in the Description page of Project Settings.


#include "EonBound_GA_Skill.h"

#include "AbilitySystemComponent.h"
#include "EonBound/GAS/EonBound_AbilitySystemComponent.h"

UEonBound_GA_Skill::UEonBound_GA_Skill()
{
	FGameplayTag SkillTag = FGameplayTag::RequestGameplayTag(FName("Ability.Skill"));
	SetAssetTags(FGameplayTagContainer(SkillTag));
	
}

bool UEonBound_GA_Skill::CommitSkill(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return false;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return false;
	}

	return true;
}

void UEonBound_GA_Skill::ApplySkillCooldown(
	const FGameplayAbilityActorInfo* ActorInfo)
{
	if (CooldownTurns <= 0 || !CooldownEffect || !CooldownTag.IsValid())
	{
		return;
	}

	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		return;
	}

	UEonBound_AbilitySystemComponent* ASC = Cast<UEonBound_AbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());

	if (!ASC)
	{
		return;
	}

	ASC->ApplyTurnCooldown(CooldownEffect,CooldownTag,CooldownTurns);
}