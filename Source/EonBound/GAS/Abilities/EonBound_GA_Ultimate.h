// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EonBound/GAS/Abilities/EonBound_GA_Skill.h"
#include "EonBound_GA_Ultimate.generated.h"

/**
 * Classe parent des ultimes. Réutilise le ciblage, la fin d'action et le cooldown de GA_Skill,
 * et ajoute un coût en énergie.
 */

UCLASS(Abstract)
class EONBOUND_API UEonBound_GA_Ultimate : public UEonBound_GA_Skill
{
	GENERATED_BODY()

public:
	UEonBound_GA_Ultimate();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	float GetEnergyCost() const { return EnergyCost; }

protected:
	// Énergie nécessaire pour lancer l'ultime (consommée à l'activation).
	UPROPERTY(EditDefaultsOnly, Category = "Ultimate", meta = (ClampMin = "0"))
	float EnergyCost = 100.f;

	// Dépense l'énergie du lanceur. Renvoie false s'il n'en a pas assez.
	bool SpendEnergy(const FGameplayAbilityActorInfo* ActorInfo);
};