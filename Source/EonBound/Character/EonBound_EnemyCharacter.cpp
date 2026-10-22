// Fill out your copyright notice in the Description page of Project Settings.

#include "EonBound_EnemyCharacter.h"

#include "EonBound/GAS/EonBound_AbilitySystemComponent.h"
#include "EonBound/GAS/EonBound_AttributeSet.h"


AEonBound_EnemyCharacter::AEonBound_EnemyCharacter()
{
	
}

void AEonBound_EnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (UEonBound_AbilitySystemComponent* ASC = EonBound_ASC)
	{
		ASC->SetNumericAttributeBase(UEonBound_AttributeSet::GetSpeedAttribute(),50.0f);
		ASC->SetNumericAttributeBase(UEonBound_AttributeSet::GetAttackAttribute(),20.0f);
	}
}
