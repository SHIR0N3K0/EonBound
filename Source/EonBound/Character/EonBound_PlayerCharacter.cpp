// Fill out your copyright notice in the Description page of Project Settings.


#include "EonBound_PlayerCharacter.h"

#include "EonBound/GAS/EonBound_AbilitySystemComponent.h"
#include "EonBound/GAS/EonBound_AttributeSet.h"


AEonBound_PlayerCharacter::AEonBound_PlayerCharacter()
{
}

void AEonBound_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (UEonBound_AbilitySystemComponent* ASC = EonBound_ASC)
	{
		ASC->SetNumericAttributeBase(UEonBound_AttributeSet::GetSpeedAttribute(),100.0f);
		ASC->SetNumericAttributeBase(UEonBound_AttributeSet::GetAttackAttribute(),75.0f);
		ASC->SetNumericAttributeBase(UEonBound_AttributeSet::GetHealthAttribute(),200.0f);
		ASC->SetNumericAttributeBase(UEonBound_AttributeSet::GetHealthMaxAttribute(),200.0f);
	}
}
