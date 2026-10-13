// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EonBound_GameMode.generated.h"

class AEonBound_PlayerCharacter;
class AEonBound_EnemyCharacter;
class AEonBound_PlayerControler;
class UEonBound_CombatManager;
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
	
	UPROPERTY(Transient)
	TObjectPtr<UEonBound_CombatManager> CombatManager;
	
public:
	TObjectPtr<UEonBound_CombatManager> GetCombatManager() const;
	
	virtual void StartPlay() override;
	
protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Spawn")
	TSubclassOf<AEonBound_PlayerCharacter> TestAllyClass;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Spawn")
	TSubclassOf<AEonBound_EnemyCharacter> TestEnemyClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Spawn")
	FTransform TestAllySpawnTransform;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Spawn")
	FTransform TestEnemySpawnTransform;
	
	UPROPERTY(Transient)
	TObjectPtr<AEonBound_PlayerCharacter> SpawnedTestAlly;

	UPROPERTY(Transient)
	TObjectPtr<AEonBound_EnemyCharacter> SpawnedTestEnemy;
	
	void SpawnTestCombatants();
	
};
