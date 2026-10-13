// Fill out your copyright notice in the Description page of Project Settings.


#include "EonBound_GA_NormalAttack.h"

#include "AbilitySystemBlueprintLibrary.h"

#include "EonBound/GAS/EonBound_AttributeSet.h"

#include "AbilitySystemComponent.h"
#include "EonBound/Character/EonBound_Character.h"

void UEonBound_GA_NormalAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	// Vérifie que l'ability possède bien les informations nécessaires pour s'exécuter.
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo,
			true, true);
		return;
	}
	
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(
			Handle,
			ActorInfo,
			ActivationInfo,
			true,
			true
		);
		return;
	}
	
	// Récupère l'attaquant, sa cible actuelle et l'effet de dégâts à appliquer.
	AEonBound_Character* Attacker =Cast<AEonBound_Character>(ActorInfo->AvatarActor.Get());

	if (!Attacker || !Attacker->CurrentTarget ||!DamageEffectClass)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo,
			true, true);
		return;
	}

	// Récupère le composant GAS de la cible afin de pouvoir lui appliquer les dégâts.
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Attacker->CurrentTarget);

	if (!TargetASC)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo,
			true, true);
		return;
	}
	
	//TEST
	if (!DamageEffectClass)
	{
		UE_LOG(LogTemp, Error, TEXT("DamageEffectClass is not assigned."));
	}

	UE_LOG(LogTemp, Warning, TEXT("Attacker: %s | Target: %s | HasAuthority: %s"),
		*GetNameSafe(Attacker),
		*GetNameSafe(Attacker ? Attacker->CurrentTarget : nullptr),
		Attacker && Attacker->HasAuthority() ? TEXT("true") : TEXT("false"));
	//TEST

	// L'effet gameplay est créé et appliqué uniquement côté serveur.
	if (Attacker->HasAuthority())
	{
		FGameplayEffectContextHandle Context = ActorInfo->AbilitySystemComponent->MakeEffectContext();

		// Construit l'effet avec le niveau actuel de l'ability et son contexte d'exécution.
		FGameplayEffectSpecHandle Spec = ActorInfo->AbilitySystemComponent->MakeOutgoingSpec(DamageEffectClass,
			GetAbilityLevel(),Context);

		if (Spec.IsValid())
		{
			//Health Before
			const float HealthBefore = TargetASC->GetNumericAttribute(UEonBound_AttributeSet::GetHealthAttribute());

			FActiveGameplayEffectHandle EffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
			UE_LOG(LogTemp, Warning, TEXT("Damage effect applied: %s"),EffectHandle.IsValid() ? TEXT("true") : TEXT("false"));
			
			/*TargetASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());*/

			//Health After
			const float HealthAfter = TargetASC->GetNumericAttribute(UEonBound_AttributeSet::GetHealthAttribute());

			UE_LOG(LogTemp, Warning, TEXT("Target Health: %.2f -> %.2f"),HealthBefore,HealthAfter);
		}
	}

	// Termine l'ability sans la marquer comme ayant été annulée.
	EndAbility(Handle,ActorInfo,ActivationInfo,true,false);
}
	
