// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EonBound/GAS/Abilities/EonBound_GA_Skill.h"
#include "EonBound_GA_Skill_Heal.generated.h"

/**
 * 
 */
UCLASS()
class EONBOUND_API UEonBound_GA_Skill_Heal : public UEonBound_GA_Skill
{
	GENERATED_BODY()
	
public:
	UEonBound_GA_Skill_Heal();
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,
								 const FGameplayAbilityActivationInfo ActivationInfo,const FGameplayEventData* TriggerEventData) override;
	
};
