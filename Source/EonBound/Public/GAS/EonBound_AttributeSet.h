// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GAS/EonBound_AbilitySystemComponent.h"
#include "EonBound_AttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
/**
 * 
 */
UCLASS()
class EONBOUND_API UEonBound_AttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UEonBound_AttributeSet();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health = 100;
	ATTRIBUTE_ACCESSORS(UEonBound_AttributeSet, Health);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_HealthMax)
	FGameplayAttributeData HealthMax = 100;
	ATTRIBUTE_ACCESSORS(UEonBound_AttributeSet, HealthMax);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Attack)
	FGameplayAttributeData Attack = 5;
	ATTRIBUTE_ACCESSORS(UEonBound_AttributeSet, Attack);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Defense)
	FGameplayAttributeData Defense = 5;
	ATTRIBUTE_ACCESSORS(UEonBound_AttributeSet, Defense);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Speed)
	FGameplayAttributeData Speed = 100;
	ATTRIBUTE_ACCESSORS(UEonBound_AttributeSet, Speed);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Energy)
	FGameplayAttributeData Energy = 0;
	ATTRIBUTE_ACCESSORS(UEonBound_AttributeSet, Energy);
	
protected : 
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);
    
	UFUNCTION()
	virtual void OnRep_HealthMax(const FGameplayAttributeData& OldHealthMax);
	
	UFUNCTION()
	virtual void OnRep_Attack(const FGameplayAttributeData& OldAttack);
	
	UFUNCTION()
	virtual void OnRep_Defense(const FGameplayAttributeData& OldDefense);
	
	UFUNCTION()
	virtual void OnRep_Speed(const FGameplayAttributeData& OldSpeed);
	
	UFUNCTION()
	virtual void OnRep_Energy(const FGameplayAttributeData& OldEnergy);
    	
};
