// Fill out your copyright notice in the Description page of Project Settings.


#include "EonBound_MMC_AttackDamage.h"
#include "EonBound/GAS/EonBound_AttributeSet.h"
#include "AbilitySystemComponent.h"

UEonBound_MMC_AttackDamage::UEonBound_MMC_AttackDamage()
{
	// Capture the Attack attribute from the source (the attacker) for damage calculation.
	AttackCapture = FGameplayEffectAttributeCaptureDefinition(UEonBound_AttributeSet::GetAttackAttribute(),
		EGameplayEffectAttributeCaptureSource::Source,true);

	// Add the Attack attribute capture to the list of relevant attributes to capture for this magnitude calculation.
	RelevantAttributesToCapture.Add(AttackCapture);
}

float UEonBound_MMC_AttackDamage::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Initialize the evaluation parameters for the attribute capture.
	FAggregatorEvaluateParameters EvaluationParameters;

	float AttackValue = 0.f;

	// Get the captured Attack attribute value from the source (attacker) and store it in AttackValue.
	GetCapturedAttributeMagnitude(AttackCapture,Spec,EvaluationParameters,AttackValue);

	// Log the calculated Attack value for debugging purposes.
	UE_LOG(LogTemp, Warning, TEXT("Calculated Attack Value: %f"), AttackValue);
	return AttackValue;
}
