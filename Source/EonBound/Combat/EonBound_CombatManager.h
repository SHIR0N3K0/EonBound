// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EonBound_CombatState.h"
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
	UEonBound_CombatManager();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StartCombat();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void RequestPlayerBasicAttack(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void NotifyActionFinished();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void CheckCombatEnd();

	UFUNCTION(BlueprintPure, Category = "Combat")
	EEonBound_CombatState GetCombatState() const;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	EEonBound_CombatState CombatState = EEonBound_CombatState::Preparing;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<AActor> ActiveCombatant;
	
public:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Combat")
	TArray<TObjectPtr<AActor>> PlayerCombatants;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Combat")
	TArray<TObjectPtr<AActor>> EnemyCombatants;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<AActor> PlayerTarget;

private:
	void SetCombatState(EEonBound_CombatState NewState);
	void SelectNextCombatant();
	void StartEnemyTurn();
	bool IsCombatantAlive(AActor* Combatant) const;
	bool bNextTurnIsPlayer = true;
};
