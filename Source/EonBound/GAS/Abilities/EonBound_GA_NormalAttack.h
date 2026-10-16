// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EonBound/GAS/EonBound_GameplayAbility.h"
#include "EonBound_GA_NormalAttack.generated.h"

/**
 * 
 */
UCLASS()
class EONBOUND_API UEonBound_GA_NormalAttack : public UEonBound_GameplayAbility
{
	GENERATED_BODY()

public:
	UEonBound_GA_NormalAttack();
	
protected:
	UPROPERTY()
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,const FGameplayEventData* TriggerEventData) override;
	
	
	
	//Delay timer handle to delay the end of the ability to allow the animation to play before ending the ability.
	FTimerHandle DelayTimerHandle;
	
	void OnDelayTimerFinished();
	
	const FGameplayAbilityActorInfo* ActorInfoCopy = nullptr;
};
