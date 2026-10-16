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
	
	CombatManager = NewObject<UEonBound_CombatManager>(this);
	
	SpawnTestCombatants();
	
	if (IsValid(SpawnedTestAlly) && IsValid(SpawnedTestEnemy))
	{
		//CombatManager->PlayerCombatants.Add(SpawnedTestAlly);
		//CombatManager->EnemyCombatants.Add(SpawnedTestEnemy);
		CombatManager->StartCombat();
	}
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

		for (int32 i = 0; i < 2; i++)
		{
			SpawnedTestAlly = World->SpawnActor<AEonBound_PlayerCharacter>(TestAllyClass,TestAllySpawnTransform,SpawnParams);
			CombatManager->PlayerCombatants.Add(SpawnedTestAlly);

			if (SpawnedTestAlly)
			{
				UE_LOG(LogTemp,Log,TEXT("Allie spawn : %s"),*SpawnedTestAlly->GetName());
			}
		}
	}

	// Spawn de l'ennemi
	if (TestEnemyClass)
	{
		FActorSpawnParameters SpawnParams;

		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		for (int32 i = 0; i < 2; i++)
		{
			SpawnedTestEnemy = World->SpawnActor<AEonBound_EnemyCharacter>(TestEnemyClass,TestEnemySpawnTransform,SpawnParams);
			CombatManager->EnemyCombatants.Add(SpawnedTestEnemy);
			if (SpawnedTestEnemy)
			{
				UE_LOG(LogTemp,Log,TEXT("Ennemi spawn : %s"),*SpawnedTestEnemy->GetName());
			}
		}
	}

	// Résumé du résultat
	UE_LOG(LogTemp,Log,TEXT("Resultat spawn - Allie : %s | Ennemi : %s"),
		SpawnedTestAlly ? TEXT("OK") : TEXT("ECHEC"),SpawnedTestEnemy ? TEXT("OK") : TEXT("ECHEC"));
}
