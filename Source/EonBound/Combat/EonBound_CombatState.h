// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EonBound_CombatState.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EEonBound_CombatState : uint8
{
	Preparing,
	WaitingForTurn,
	PlayerTurn,
	EnemyTurn,
	ExecutingAction,
	Victory,
	Defeat
};