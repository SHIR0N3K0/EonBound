// Fill out your copyright notice in the Description page of Project Settings.


#include "EonBound/GAS/Abilities/SkillChild/GA_Ultimate_DamageAll.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EonBound/Character/EonBound_Character.h"
#include "EonBound/GAS/EonBound_AttributeSet.h"
#include "EonBound/GAS/Effects/Combat/EonBound_GE_Attack.h"

UGA_Ultimate_DamageAll::UGA_Ultimate_DamageAll()
{
	// Ciblage : tous les ennemis.
	Targeting.Team = ESkillTargetTeam::Enemy;
	Targeting.Pattern = ESkillTargetPattern::All;

	// Un ultime ne redonne pas d'énergie (réglable dans le Blueprint).
	EnergyGain = 0.f;

	DamageEffectClass = UEonBound_GE_Attack::StaticClass();
}

void UGA_Ultimate_DamageAll::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AEonBound_Character* Caster = Cast<AEonBound_Character>(ActorInfo->AvatarActor.Get());

	if (!Caster || Caster->GetCurrentActionTargets().IsEmpty() || !DamageEffectClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[Ultimate] Caster, cibles ou DamageEffectClass manquants."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// CommitSkill termine déjà l'ability en cas d'échec.
	if (!CommitSkill(Handle, ActorInfo, ActivationInfo))
	{
		return;
	}

	// Paiement du coût en énergie.
	if (!SpendEnergy(ActorInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilitySystemComponent* CasterASC = ActorInfo->AbilitySystemComponent.Get();

	const float Attack = CasterASC->GetNumericAttribute(UEonBound_AttributeSet::GetAttackAttribute());
	const float CritRate = FMath::Clamp(CasterASC->GetNumericAttribute(UEonBound_AttributeSet::GetCritChanceAttribute()), 0.f, 100.f);
	const float CritDamage = FMath::Max(CasterASC->GetNumericAttribute(UEonBound_AttributeSet::GetCritDamageAttribute()), 0.f);

	const FGameplayTag DamageTag = FGameplayTag::RequestGameplayTag(FName("Data.Damage"));
	const FGameplayTag CriticalTag = FGameplayTag::RequestGameplayTag(FName("Data.IsCritical"));

	if (Caster->HasAuthority())
	{
		// Copie des cibles : la liste du personnage sera vidée à la fin de l'action.
		for (AEonBound_Character* Target : TArray<TObjectPtr<AEonBound_Character>>(Caster->GetCurrentActionTargets()))
		{
			if (!IsValid(Target) || Target->IsDead())
			{
				continue;
			}

			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);

			if (!TargetASC)
			{
				continue;
			}

			// Un jet de critique par cible.
			const bool bIsCritical = FMath::FRandRange(0.f, 100.f) < CritRate;
			const float FinalDamage = Attack * DamageMultiplier * (bIsCritical ? CritDamage / 100.f : 1.f);

			FGameplayEffectSpecHandle Spec = CasterASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(),
				CasterASC->MakeEffectContext());

			if (!Spec.IsValid())
			{
				continue;
			}

			Spec.Data->SetSetByCallerMagnitude(DamageTag, FinalDamage);
			Spec.Data->SetSetByCallerMagnitude(CriticalTag, bIsCritical ? 1.f : 0.f);
			TargetASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());

			UE_LOG(LogTemp, Warning, TEXT("[Ultimate] %s : dégâts %.2f | critique : %s"),
				*Target->GetName(), FinalDamage, bIsCritical ? TEXT("OUI") : TEXT("NON"));
		}
	}

	ApplySkillCooldown(ActorInfo);
	FinishSkillAction();
}