// Fill out your copyright notice in the Description page of Project Settings.


#include "EonBound_GA_NormalAttack.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "EonBound/GAS/Effects/EonBound_GE_Attack.h"
#include "AbilitySystemComponent.h"
#include "EonBound/Character/EonBound_Character.h"
#include "EonBound/Combat/EonBound_CombatManager.h"

UEonBound_GA_NormalAttack::UEonBound_GA_NormalAttack()
{
	DamageEffectClass = UEonBound_GE_Attack::StaticClass();
	//New API: Set the ability's asset tags to "Ability.BasicAttack" for identification and filtering purposes.
	FGameplayTag BasicAttackTag = FGameplayTag::RequestGameplayTag(FName("Ability.BasicAttack"));
	SetAssetTags(FGameplayTagContainer(BasicAttackTag));

}

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
			TargetASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
		}
	}
	
	//Delay the end of the ability to allow the animation to play before ending the ability.
	ActorInfoCopy = ActorInfo;
	GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle,this, &UEonBound_GA_NormalAttack::OnDelayTimerFinished,2.0f,false);
	
}

void UEonBound_GA_NormalAttack::OnDelayTimerFinished()
{
	UE_LOG(LogTemp, Warning,TEXT("[Combat] NormalAttack: EndAbility called."));
	
	if (AEonBound_Character* AttackingChar = Cast<AEonBound_Character>(ActorInfoCopy->AvatarActor.Get()))
	{
		UE_LOG(LogTemp, Warning,TEXT("[Combat] Attacker: %s | Manager: %s"),*AttackingChar->GetName(),
			AttackingChar->GetCombatManager()? TEXT("Valid"): TEXT("NULL"));
		
		if (AttackingChar->GetCombatManager())
		{
			UE_LOG(LogTemp, Warning,TEXT("[Combat] Calling NotifyActionFinished."));
			AttackingChar->GetCombatManager()->NotifyActionFinished();
		}
		
	}
	
	// Termine l'ability sans la marquer comme ayant été annulée.
	EndAbility(CurrentSpecHandle, ActorInfoCopy, CurrentActivationInfo, true, false);
}
	
