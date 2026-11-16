// Fill out your copyright notice in the Description page of Project Settings.


#include "EonBound_GA_Skill_Heal.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EonBound/Character/EonBound_Character.h"
#include "EonBound/GAS/EonBound_AttributeSet.h"
#include "EonBound/GAS/Effects/Combat/EonBound_GE_Heal.h"

UEonBound_GA_Skill_Heal::UEonBound_GA_Skill_Heal()
{
	FGameplayTagContainer SkillTags;

	SkillTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill")));

	SkillTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Heal")));

	SetAssetTags(SkillTags);
	
	// Cooldown
	CooldownTag = FGameplayTag::RequestGameplayTag(FName("Cooldown.Skill.Heal"));

	ActivationBlockedTags.AddTag(CooldownTag);
	
	HealEffectClass = UEonBound_GE_Heal::StaticClass();
}

void UEonBound_GA_Skill_Heal::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	// Vérification des conditions d'activation
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AEonBound_Character* Caster = Cast<AEonBound_Character>(ActorInfo->AvatarActor.Get());

	// Vérification des cibles et de la classe d'effet de soin
	if (!Caster || Caster->GetCurrentActionTargets().IsEmpty() || !HealEffectClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[Heal] Caster, cibles ou HealEffectClass manquants."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// CommitSkill termine déjà l'ability en cas d'échec.
	if (!CommitSkill(Handle, ActorInfo, ActivationInfo))
	{
		return;
	}

	UAbilitySystemComponent* CasterASC = ActorInfo->AbilitySystemComponent.Get();
	const FGameplayTag HealTag = FGameplayTag::RequestGameplayTag(FName("Data.Heal"));

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

			const float Health = TargetASC->GetNumericAttribute(UEonBound_AttributeSet::GetHealthAttribute());
			const float HealthMax = TargetASC->GetNumericAttribute(UEonBound_AttributeSet::GetHealthMaxAttribute());

			if (HealthMax <= 0.f)
			{
				continue;
			}

			// Soin = base × (1 + bonus × part de PV manquants).
			const float Missing = FMath::Max(HealthMax - Health, 0.f);
			const float HealAmount = BaseHeal * (1.f + MissingHealthBonus * Missing / HealthMax);
			const float ActualHeal = FMath::Min(HealAmount, Missing);

			FGameplayEffectSpecHandle Spec = CasterASC->MakeOutgoingSpec(HealEffectClass, GetAbilityLevel(),
				CasterASC->MakeEffectContext());

			if (!Spec.IsValid())
			{
				continue;
			}

			Spec.Data->SetSetByCallerMagnitude(HealTag, HealAmount);
			TargetASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());

			UE_LOG(LogTemp, Warning, TEXT("[Heal] %s : %.2f/%.2f PV | soin calculé %.2f | soin réel %.2f"),
				*Target->GetName(), Health, HealthMax, HealAmount, ActualHeal);

			// Nombre négatif = affichage vert.
			if (ActualHeal > 0.f)
			{
				Target->SpawnDamageNumber(-ActualHeal, false);
			}
		}
	}

	ApplySkillCooldown(ActorInfo);
	FinishSkillAction();
} 
