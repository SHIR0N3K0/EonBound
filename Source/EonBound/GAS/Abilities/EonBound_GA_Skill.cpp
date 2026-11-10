// Fill out your copyright notice in the Description page of Project Settings.


#include "EonBound_GA_Skill.h"

#include "TimerManager.h"
#include "Engine/World.h"
#include "EonBound/Character/EonBound_Character.h"
#include "EonBound/Combat/EonBound_CombatManager.h"
#include "AbilitySystemComponent.h"
#include "EonBound/GAS/EonBound_AbilitySystemComponent.h"

UEonBound_GA_Skill::UEonBound_GA_Skill()
{
	FGameplayTag SkillTag = FGameplayTag::RequestGameplayTag(FName("Ability.Skill"));
	SetAssetTags(FGameplayTagContainer(SkillTag));
	
}

bool UEonBound_GA_Skill::CommitSkill(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return false;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return false;
	}

	return true;
}

void UEonBound_GA_Skill::ApplySkillCooldown(
	const FGameplayAbilityActorInfo* ActorInfo)
{
	if (CooldownTurns <= 0 || !CooldownEffect || !CooldownTag.IsValid())
	{
		return;
	}

	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		return;
	}

	UEonBound_AbilitySystemComponent* ASC = Cast<UEonBound_AbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());

	if (!ASC)
	{
		return;
	}

	ASC->ApplyTurnCooldown(CooldownEffect,CooldownTag,CooldownTurns);
}

bool UEonBound_GA_Skill::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	// Cooldown en tours : la source de vérité est la liste de l'ASC.
	if (CooldownTag.IsValid() && ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
	{
		const UEonBound_AbilitySystemComponent* ASC = Cast<UEonBound_AbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());

		if (ASC && ASC->HasTurnCooldown(CooldownTag))
		{
			if (OptionalRelevantTags)
			{
				OptionalRelevantTags->AddTag(CooldownTag);
			}
			return false;
		}
	}

	return true;
}

void UEonBound_GA_Skill::FinishSkillAction(float Delay)
{
	UWorld* World = GetWorld();

	if (!World)
	{
		OnFinishSkillActionTimer();
		return;
	}

	World->GetTimerManager().SetTimer(FinishActionTimerHandle, this, &UEonBound_GA_Skill::OnFinishSkillActionTimer, Delay, false);
}

void UEonBound_GA_Skill::OnFinishSkillActionTimer()
{
	AEonBound_Character* Caster = CurrentActorInfo ? Cast<AEonBound_Character>(CurrentActorInfo->AvatarActor.Get()) : nullptr;

	if (Caster && Caster->GetCombatManager())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Combat] Skill %s: NotifyActionFinished."), *GetNameSafe(this));
		Caster->GetCombatManager()->NotifyActionFinished();
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}