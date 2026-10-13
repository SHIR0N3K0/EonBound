// Fill out your copyright notice in the Description page of Project Settings.

#include "EonBound_GameMode.h"
#include "EonBound/Character/EonBound_PlayerCharacter.h"
#include "EonBound/Character/EonBound_EnemyCharacter.h"
#include "EonBound/Combat/EonBound_CombatManager.h"

AEonBound_GameMode::AEonBound_GameMode()
{	
}

TObjectPtr<UEonBound_CombatManager> AEonBound_GameMode::GetCombatManager() const
{
	return CombatManager;
}

void AEonBound_GameMode::StartPlay()
{
	Super::StartPlay();
	
	SpawnTestCombatants();
	CombatManager = NewObject<UEonBound_CombatManager>(this);
}

void AEonBound_GameMode::SpawnTestCombatants()
{
	if (!HasAuthority())
	{
		return;
	}

	UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}

	// Spawn de l'allié
	if (TestAllyClass)
	{
		FActorSpawnParameters SpawnParams;

		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		SpawnedTestAlly = World->SpawnActor<AEonBound_PlayerCharacter>(TestAllyClass,TestAllySpawnTransform,SpawnParams);

		if (SpawnedTestAlly)
		{
			UE_LOG(LogTemp,Log,TEXT("Allie spawn : %s"),*SpawnedTestAlly->GetName());
		}
	}

	// Spawn de l'ennemi
	if (TestEnemyClass)
	{
		FActorSpawnParameters SpawnParams;

		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		SpawnedTestEnemy = World->SpawnActor<AEonBound_EnemyCharacter>(TestEnemyClass,TestEnemySpawnTransform,SpawnParams);

		if (SpawnedTestEnemy)
		{
			UE_LOG(LogTemp,Log,TEXT("Ennemi spawn : %s"),*SpawnedTestEnemy->GetName());
		}
	}

	// Résumé du résultat
	UE_LOG(LogTemp,Log,TEXT("Resultat spawn - Allie : %s | Ennemi : %s"),
		SpawnedTestAlly ? TEXT("OK") : TEXT("ECHEC"),SpawnedTestEnemy ? TEXT("OK") : TEXT("ECHEC"));
	
	// Assignation des cibles : les deux personnages existent maintenant.
	if (IsValid(SpawnedTestAlly) && IsValid(SpawnedTestEnemy))
	{
		SpawnedTestAlly->CurrentTarget = SpawnedTestEnemy;
		SpawnedTestEnemy->CurrentTarget = SpawnedTestAlly;

		UE_LOG(LogTemp, Log, TEXT("Cibles assignees correctement."));
	}
}
