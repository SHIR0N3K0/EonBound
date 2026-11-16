// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EonBound/GAS/Abilities/EonBound_GA_Skill.h"
#include "EonBound_GA_Skill_Heal.generated.h"

class UGameplayEffect;
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
	
	UPROPERTY(EditDefaultsOnly, Category = "Heal")
	TSubclassOf<UGameplayEffect> HealEffectClass;

	// Soin de base, avant le bonus de PV manquants.
	UPROPERTY(EditDefaultsOnly, Category = "Heal", meta = (ClampMin = "0"))
	float BaseHeal = 50.f;

	// 1 = jusqu'à +100 % de soin sur une cible presque morte.
	UPROPERTY(EditDefaultsOnly, Category = "Heal", meta = (ClampMin = "0"))
	float MissingHealthBonus = 1.f;
	
	
};
