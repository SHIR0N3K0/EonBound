// Fill out your copyright notice in the Description page of Project Settings.

#include "EonBound_GameMode.h"

#include "EonBound_PlayerControler.h"
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
	
	if (!IsValid(CombatManager))
	{
		UE_LOG(LogTemp, Error, TEXT("[GameMode] CombatManager creation failed."));
		return;
	}

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	EonBound_PC = Cast<AEonBound_PlayerControler>(PlayerController);

	if (!IsValid(EonBound_PC))
	{
		UE_LOG(LogTemp, Error,TEXT("[GameMode] PlayerController is invalid or has the wrong class. Actual: %s"),
			*GetNameSafe(PlayerController));
		return;
	}

	EonBound_PC->SetManagerREF(CombatManager);
	
	
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

		for (int32 i = 0; i < 1; i++)
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
				SpawnedTestEnemy->SetTargetIndicationVisibility(false);
				UE_LOG(LogTemp,Log,TEXT("Ennemi spawn : %s"),*SpawnedTestEnemy->GetName());
			}
		}
	}

	// Résumé du résultat
	UE_LOG(LogTemp,Log,TEXT("Resultat spawn - Allie : %s | Ennemi : %s"),
		SpawnedTestAlly ? TEXT("OK") : TEXT("ECHEC"),SpawnedTestEnemy ? TEXT("OK") : TEXT("ECHEC"));
}
