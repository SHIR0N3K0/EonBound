// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EonBound_GameMode.generated.h"

class AEonBound_PlayerCharacter;
class AEonBound_EnemyCharacter;
class AEonBound_PlayerControler;
class UEonBound_CombatManager;
class UCombatHUD_UserWidget;
/**
 * 
 */
UCLASS()
class EONBOUND_API AEonBound_GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	AEonBound_GameMode();
	
private:
	UPROPERTY()
	TObjectPtr<AEonBound_PlayerControler> EonBound_PC;
	
public:

	TObjectPtr<UEonBound_CombatManager> GetCombatManager() const;
	
	virtual void StartPlay() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat|UI")
	TSubclassOf<UCombatHUD_UserWidget> CombatHUDClassToGiveToManager;
	
protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Spawn")
	TSubclassOf<AEonBound_PlayerCharacter> TestAllyClass;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Spawn")
	TSubclassOf<AEonBound_EnemyCharacter> TestEnemyClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Spawn")
	int32 TestAllyCount = 2;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Spawn")
	int32 TestEnemyCount = 4;
	
	void SpawnTestCombatants();
	
	UPROPERTY()
	TObjectPtr<UEonBound_CombatManager> CombatManager;
	
};
