// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "EonBound/GAS/EonBound_AbilitySystemComponent.h"
#include "EonBound_AttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class EONBOUND_API UEonBound_AttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UEonBound_AttributeSet();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CustomAbilitySystemComponent, Replicated)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS_BASIC(UEonBound_AttributeSet, Health);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CustomAbilitySystemComponent, Replicated)
	FGameplayAttributeData HealthMax;
	ATTRIBUTE_ACCESSORS_BASIC(UEonBound_AttributeSet, HealthMax);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CustomAbilitySystemComponent, Replicated)
	FGameplayAttributeData Defense;
	ATTRIBUTE_ACCESSORS_BASIC(UEonBound_AttributeSet, Defense);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CustomAbilitySystemComponent, Replicated)
	FGameplayAttributeData Speed;
	ATTRIBUTE_ACCESSORS_BASIC(UEonBound_AttributeSet, Speed);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CustomAbilitySystemComponent, Replicated)
	FGameplayAttributeData ATB;
	ATTRIBUTE_ACCESSORS_BASIC(UEonBound_AttributeSet, ATB);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CustomAbilitySystemComponent, Replicated)
	FGameplayAttributeData Energy;
	ATTRIBUTE_ACCESSORS_BASIC(UEonBound_AttributeSet, Energy);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CustomAbilitySystemComponent, Replicated)
	FGameplayAttributeData BalancingCoefficient;
	ATTRIBUTE_ACCESSORS_BASIC(UEonBound_AttributeSet, BalancingCoefficient);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CustomAbilitySystemComponent, Replicated)
	FGameplayAttributeData CritChance;
	ATTRIBUTE_ACCESSORS_BASIC(UEonBound_AttributeSet, CritChance);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CustomAbilitySystemComponent, Replicated)
	FGameplayAttributeData CritDamage;
	ATTRIBUTE_ACCESSORS_BASIC(UEonBound_AttributeSet, CritDamage);
	
	UPROPERTY(EditAnywhere)
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS_BASIC(UEonBound_AttributeSet, Damage);
	
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
    	
};
