// Fill out your copyright notice in the Description page of Project Settings.


#include "Systems/EonBound_PlayerState.h"
#include "GAS/EonBound_AbilitySystemComponent.h"
#include "GAS/EonBound_AttributeSet.h"

AEonBound_PlayerState::AEonBound_PlayerState()
{
	EonBound_ASC = CreateDefaultSubobject<UEonBound_AbilitySystemComponent>("EonBound_ASC");
	EonBound_AttributeSet = CreateDefaultSubobject<UEonBound_AttributeSet>("EonBound_AttributeSet");
}

UAbilitySystemComponent* AEonBound_PlayerState::GetAbilitySystemComponent() const
{
	return EonBound_ASC;
}

void AEonBound_PlayerState::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority() && EonBound_ASC)
	{
		
	}
}