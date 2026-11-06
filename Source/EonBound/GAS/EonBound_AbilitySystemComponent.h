// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "EonBound_AbilitySystemComponent.generated.h"

struct FTurnBasedCooldown
{
	FActiveGameplayEffectHandle EffectHandle;

	FGameplayTag CooldownTag;

	int32 RemainingTurns = 0;
};

/**
 * 
 */

UCLASS()
class EONBOUND_API UEonBound_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:

	// Cooldown management
	bool ApplyTurnCooldown(TSubclassOf<UGameplayEffect> CooldownEffect,FGameplayTag CooldownTag,int32 DurationInTurns);

	void AdvanceTurnCooldowns();

private:

	TArray<FTurnBasedCooldown> ActiveTurnCooldowns;
};