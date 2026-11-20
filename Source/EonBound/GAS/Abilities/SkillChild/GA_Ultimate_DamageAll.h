// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EonBound/GAS/Abilities/EonBound_GA_Ultimate.h"
#include "GA_Ultimate_DamageAll.generated.h"

class UGameplayEffect;

/**
 * Ultime : inflige des dégâts à tous les ennemis (Attaque × multiplicateur), avec un jet de critique par cible.
 */

UCLASS()
class EONBOUND_API UGA_Ultimate_DamageAll : public UEonBound_GA_Ultimate
{
	GENERATED_BODY()
	
public:
	UGA_Ultimate_DamageAll();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, Category = "Ultimate|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	// Dégâts = Attaque × ce multiplicateur (avant critique).
	UPROPERTY(EditDefaultsOnly, Category = "Ultimate|Damage", meta = (ClampMin = "0"))
	float DamageMultiplier = 2.f;
	
};
