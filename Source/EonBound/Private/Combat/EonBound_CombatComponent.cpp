// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/EonBound_CombatComponent.h"

// Sets default values
UEonBound_CombatComponent::UEonBound_CombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;	
}

// Called when the game starts or when spawned
void UEonBound_CombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
}
void UEonBound_CombatComponent::EnterCombat()
{
	bIsInCombat = true;
}

void UEonBound_CombatComponent::LeaveCombat()
{
	bIsInCombat = false;
	bIsTakingTurn = false;
}

void UEonBound_CombatComponent::StartTurn()
{
	bIsTakingTurn = true;
}

void UEonBound_CombatComponent::EndTurn()
{
	bIsTakingTurn = false;
}

bool UEonBound_CombatComponent::IsInCombat() const
{
	return bIsInCombat;
}

bool UEonBound_CombatComponent::IsTakingTurn() const
{
	return bIsTakingTurn;
}