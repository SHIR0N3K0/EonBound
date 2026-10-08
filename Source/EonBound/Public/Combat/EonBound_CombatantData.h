// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "EonBound_CombatantData.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FCombatantDefinition
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CharacterID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Attack = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Defense = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed = 10.0f;
};

USTRUCT(BlueprintType)
struct FCombatantState
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsDead = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool CanAct = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 TurnIndex;
};