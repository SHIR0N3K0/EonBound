// Fill out your copyright notice in the Description page of Project Settings.


#include "EonBound_AttributeSet.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "EonBound/Character/EonBound_Character.h"

UEonBound_AttributeSet::UEonBound_AttributeSet()
{
	InitHealth(100);
	InitHealthMax(100);
	InitAttack(20);
	InitDefense(50);
	InitSpeed(200);
	InitATB(1);
	InitEnergy(100);
	InitDamage(0);
	InitBalancingCoefficient(150);
	InitCritChance(10);
	InitCritDamage(120);
}

void UEonBound_AttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);		
	DOREPLIFETIME(UEonBound_AttributeSet, Health);
	DOREPLIFETIME(UEonBound_AttributeSet, HealthMax);
	DOREPLIFETIME(UEonBound_AttributeSet, Attack);
	DOREPLIFETIME(UEonBound_AttributeSet, Defense);
	DOREPLIFETIME(UEonBound_AttributeSet, Speed);
	DOREPLIFETIME(UEonBound_AttributeSet, ATB);
	DOREPLIFETIME(UEonBound_AttributeSet, Energy);
	DOREPLIFETIME(UEonBound_AttributeSet, Damage);
	DOREPLIFETIME(UEonBound_AttributeSet, BalancingCoefficient);
	DOREPLIFETIME(UEonBound_AttributeSet, CritChance);
	DOREPLIFETIME(UEonBound_AttributeSet, CritDamage);
}

void UEonBound_AttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	// Vérifie si l'attribut modifié est "Damage" et applique les dégâts à la santé.
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float CurrentDamage = GetDamage();
		const float CurrentDefense = GetDefense();
		const float CurrentBalancingCoefficient = GetBalancingCoefficient();
		
		const float OldHealthValue = GetHealth();
		const float MaxHealthValue = GetHealthMax();
		
		const FGameplayTag CriticalTag = FGameplayTag::RequestGameplayTag(FName("Data.IsCritical"));
		const bool bIsCritical = Data.EffectSpec.GetSetByCallerMagnitude(CriticalTag,false,0.f) > 0.5f;
		
		const float NewHealthValue = FMath::Clamp
		(OldHealthValue - (CurrentDamage / (1 + CurrentDefense/CurrentBalancingCoefficient)), 0, MaxHealthValue);
		
		UE_LOG(LogTemp, Warning, TEXT("Target Health: %.2f -> %.2f"),OldHealthValue,NewHealthValue);
		
		const float ActualDamage = OldHealthValue - NewHealthValue;
		
		if (OldHealthValue != NewHealthValue)
		{
			SetHealth(NewHealthValue);
		}
		
		if (ActualDamage > 0.f)
		{
			if (AEonBound_Character* TargetCharacter =
				Cast<AEonBound_Character>(Data.Target.GetAvatarActor()))
			{
				TargetCharacter->SpawnDamageNumber(ActualDamage, bIsCritical);
			}
		}
		
		SetDamage(0);
	}
	
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetHealthMax()));
	}
}

