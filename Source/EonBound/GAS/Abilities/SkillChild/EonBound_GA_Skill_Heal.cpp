// Fill out your copyright notice in the Description page of Project Settings.


#include "EonBound_GA_Skill_Heal.h"

#include "AbilitySystemComponent.h"

UEonBound_GA_Skill_Heal::UEonBound_GA_Skill_Heal()
{
	FGameplayTagContainer SkillTags;

	SkillTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill")));

	SkillTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Heal")));

	SetAssetTags(SkillTags);
	
	// Cooldown
	CooldownTag = FGameplayTag::RequestGameplayTag(FName("Cooldown.Skill.Heal"));

	ActivationBlockedTags.AddTag(CooldownTag);
	
	//définir CooldownEffect et CooldownTurns dans les valeurs
}

void UEonBound_GA_Skill_Heal::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	

} 
