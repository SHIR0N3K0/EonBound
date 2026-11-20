// Fill out your copyright notice in the Description page of Project Settings.


#include "EonBound_GA_Ultimate.h"

#include "EonBound/Character/EonBound_Character.h"

UEonBound_GA_Ultimate::UEonBound_GA_Ultimate()
{
	// Remplace le tag Ability.Skill posé par la classe parent.
	SetAssetTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag(FName("Ability.Ultimate"))));
	EnergyGain = 0.f;
}

bool UEonBound_GA_Ultimate::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const AEonBound_Character* Caster = ActorInfo ? Cast<AEonBound_Character>(ActorInfo->AvatarActor.Get()) : nullptr;

	// Pas assez d'énergie : l'ultime n'est pas disponible.
	return Caster && Caster->GetEnergy() >= EnergyCost;
}

bool UEonBound_GA_Ultimate::SpendEnergy(const FGameplayAbilityActorInfo* ActorInfo)
{
	AEonBound_Character* Caster = ActorInfo ? Cast<AEonBound_Character>(ActorInfo->AvatarActor.Get()) : nullptr;

	if (!Caster || Caster->GetEnergy() < EnergyCost)
	{
		return false;
	}

	Caster->AddEnergy(-EnergyCost);
	return true;
}