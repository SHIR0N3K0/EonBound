// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EonBound_CombatState.h"
#include "UObject/Object.h"
#include "EonBound_CombatManager.generated.h"

class AEonBound_Character;
class UCombatHUD_UserWidget;
class APlayerController;



UENUM()
enum class ECombatTargetingMode : uint8
{
	TargetNone,
	TargetAlly,
	TargetEnemy,
	TargetAny
};

UENUM()
enum class ECombatActionMode : uint8
{
	BasicAttack,
	Skill
};

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
	void RequestPlayerBasicAttack(AEonBound_Character* AttackTarget);
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void RequestPlayerSkill(AEonBound_Character* AttackTarget);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void NotifyActionFinished();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void CheckCombatEnd();

	UFUNCTION(BlueprintPure, Category = "Combat")
	EEonBound_CombatState GetCombatState() const;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat|UI")
	TSubclassOf<UCombatHUD_UserWidget> CombatHUDClass;

	UPROPERTY(Transient)
	TObjectPtr<UCombatHUD_UserWidget> CombatHUD;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	EEonBound_CombatState CombatState = EEonBound_CombatState::Preparing;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<AEonBound_Character> ActiveCombatant = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat",meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<AEonBound_Character>> TurnOrder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat",meta = (AllowPrivateAccess = "true"))
	int32 CurrentTurnIndex = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat",meta = (AllowPrivateAccess = "true"))
	int32 RoundNumber = 1;
	
	// Timer handle to delay the start of the next turn after an action is finished.
	FTimerHandle DelayTimerHandle;
	
	void OnDelayTimerFinished();
	
	void ClearCombatTimers();
	
	
public:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Combat")
	TArray<TObjectPtr<AEonBound_Character>> PlayerCombatants;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Combat")
	TArray<TObjectPtr<AEonBound_Character>> EnemyCombatants;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<AEonBound_Character> SelectedTarget = nullptr;
	
	void SetSelectedTarget(AEonBound_Character* RequestedTarget);
	
	void BeginTargeting(ECombatTargetingMode NewTargetingMode);

	void CancelTargeting();
	
	UPROPERTY()
	int32 CurrentWaveIndex = 0;

	UPROPERTY()
	int32 TotalWaves = 1;
	
	void ToggleSkillTargeting();
	
	ECombatTargetingMode GetTargetingMode() const
	{
		return TargetingMode;
	}

private:
	
	void SetCombatState(EEonBound_CombatState NewState);
	void StartEnemyTurn();
	bool IsCombatantAlive(AEonBound_Character* Combatant) const;
	void BuildTurnOrder();
	float GetCombatantSpeed(const AEonBound_Character* Combatant) const;
	void AdvanceTurn();
	void StartCurrentTurn(bool bIsNewRound = false);
	void SelectFirstLivingEnemy();
	bool ValidateCombatants();
	
	ECombatTargetingMode TargetingMode = ECombatTargetingMode::TargetEnemy;
};
