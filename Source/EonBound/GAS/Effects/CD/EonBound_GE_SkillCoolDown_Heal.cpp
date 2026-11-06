// Fill out your copyright notice in the Description page of Project Settings.


#include "EonBound/GAS/Effects/CD/EonBound_GE_SkillCoolDown_Heal.h"

#include "GameplayTagContainer.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"

UEonBound_GE_SkillCoolDown_Heal::UEonBound_GE_SkillCoolDown_Heal()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	
	{
		UTargetTagsGameplayEffectComponent* TargetTagsComponent =CreateDefaultSubobject<UTargetTagsGameplayEffectComponent>(TEXT("TargetTagsComponent"));

		FInheritedTagContainer TagContainer;

		TagContainer.Added.AddTag(FGameplayTag::RequestGameplayTag(FName("Cooldown.Skill.Heal")));

		TargetTagsComponent->SetAndApplyTargetTagChanges(TagContainer);
	}
}