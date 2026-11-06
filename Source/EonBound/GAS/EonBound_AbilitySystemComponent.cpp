// Fill out your copyright notice in the Description page of Project Settings.


#include "EonBound_AbilitySystemComponent.h"

bool UEonBound_AbilitySystemComponent::ApplyTurnCooldown(TSubclassOf<UGameplayEffect> CooldownEffect,
	FGameplayTag CooldownTag, int32 DurationInTurns)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return false;
	}

	if (!CooldownEffect || !CooldownTag.IsValid() || DurationInTurns <= 0)
	{
		return false;
	}
	
	// Création du Gameplay Effect Spec

	FGameplayEffectContextHandle EffectContext = MakeEffectContext();

	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(CooldownEffect,1.0f,EffectContext);

	if (!SpecHandle.IsValid())
	{
		return false;
	}


	// Application du Gameplay Effect sur soi-même

	FActiveGameplayEffectHandle ActiveEffectHandle = ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);

	if (!ActiveEffectHandle.IsValid())
	{
		return false;
	}


	// Enregistrement du cooldown

	FTurnBasedCooldown NewCooldown;

	NewCooldown.EffectHandle = ActiveEffectHandle;
	NewCooldown.CooldownTag = CooldownTag;
	NewCooldown.RemainingTurns = DurationInTurns;

	ActiveTurnCooldowns.Add(NewCooldown);

	return true;
}

void UEonBound_AbilitySystemComponent::AdvanceTurnCooldowns()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	for (int32 i = ActiveTurnCooldowns.Num() - 1; i >= 0; --i)
	{
		FTurnBasedCooldown& Cooldown = ActiveTurnCooldowns[i];

		Cooldown.RemainingTurns--;

		if (Cooldown.RemainingTurns <= 0)
		{
			RemoveActiveGameplayEffect(Cooldown.EffectHandle);

			ActiveTurnCooldowns.RemoveAt(i);
		}
	}
}

