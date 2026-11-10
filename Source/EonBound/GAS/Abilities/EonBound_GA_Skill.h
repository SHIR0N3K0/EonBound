// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "EonBound_SkillTypes.h"

#include "EonBound_GA_Skill.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class EONBOUND_API UEonBound_GA_Skill : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UEonBound_GA_Skill();
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
	const FSkillTargeting& GetTargeting() const { return Targeting; }
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	FSkillTargeting Targeting;
	
	// Cooldown
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	TSubclassOf<UGameplayEffect> CooldownEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Cooldown", meta = (ClampMin = "0"))
	int32 CooldownTurns = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	FGameplayTag CooldownTag;


	// Activation
	bool CommitSkill(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo);

	// Cooldown
	void ApplySkillCooldown(const FGameplayAbilityActorInfo* ActorInfo);
	
	// Fin d'action commune : attend Delay secondes, prévient le CombatManager, puis termine l'ability.
	void FinishSkillAction(float Delay = 2.0f);

private:
	void OnFinishSkillActionTimer();

	FTimerHandle FinishActionTimerHandle;
};
