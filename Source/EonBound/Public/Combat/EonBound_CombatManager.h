// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EonBound_CombatManager.generated.h"

class UEonBound_CombatComponent;

/**
 * 
 */
UCLASS()
class EONBOUND_API UEonBound_CombatManager : public UObject
{
	GENERATED_BODY()
	
public:
	void StartCombat();
	void EndCombat();

	void StartTurn();
	void EndTurn();
	void NextTurn();

	void RegisterCombatant(UEonBound_CombatComponent* Combatant);
	void UnregisterCombatant(UEonBound_CombatComponent* Combatant);
	
private:

	UPROPERTY()
	TArray<TObjectPtr<UEonBound_CombatComponent>> Combatants;

	UPROPERTY()
	TArray<TObjectPtr<UEonBound_CombatComponent>> TurnOrder;

	UPROPERTY()
	TObjectPtr<UEonBound_CombatComponent> CurrentCombatant;
};
