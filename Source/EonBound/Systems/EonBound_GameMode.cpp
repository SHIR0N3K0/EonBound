// Fill out your copyright notice in the Description page of Project Settings.

#include "EonBound_GameMode.h"

#include "EonBound_PlayerControler.h"
#include "EonBound/Character/EonBound_PlayerCharacter.h"
#include "EonBound/Character/EonBound_EnemyCharacter.h"
#include "EonBound/Combat/EonBound_CombatManager.h"
#include "EonBound/UI/CombatHUD_UserWidget.h"
#include "EonBound/Combat/EonBound_CombatSpawnPoint.h"
#include "EngineUtils.h"

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
	
	if (!IsValid(CombatHUDClassToGiveToManager))
	{
		UE_LOG(LogTemp, Error, TEXT("[GameMode] CombatHUDClassToGiveToManager is not assigned."));
		return;
	}
	
	CombatManager->CombatHUDClass = CombatHUDClassToGiveToManager;

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
	
	if (CombatManager->PlayerCombatants.Num() > 0 &&
	CombatManager->EnemyCombatants.Num() > 0)
	{
		CombatManager->StartCombat();
	}
	else
	{
		UE_LOG(LogTemp, Error,
			TEXT("[GameMode] Combat cannot start: missing combatants."));
	}
}

void AEonBound_GameMode::SpawnTestCombatants()
{
	if (!HasAuthority() || !IsValid(CombatManager))
	{
		return;
	}

	UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}

	TArray<AEonBound_CombatSpawnPoint*> AllyPoints;
	TArray<AEonBound_CombatSpawnPoint*> EnemyPoints;
	
	for (TActorIterator<AEonBound_CombatSpawnPoint> It(World);It;++It)
	{
		AEonBound_CombatSpawnPoint* Point = *It;

		if (!IsValid(Point))
		{
			continue;
		}

		switch (Point->Team)
		{
		case ECombatSpawnTeam::Ally:AllyPoints.Add(Point);
			break;

		case ECombatSpawnTeam::Enemy:EnemyPoints.Add(Point);
			break;
		}
	}

	// Trier les points de spawn par ordre croissant de SpawnOrder
	auto SortBySpawnOrder = [](const AEonBound_CombatSpawnPoint& A,const AEonBound_CombatSpawnPoint& B)
	{
		return A.SpawnOrder < B.SpawnOrder;
	};
	
	AllyPoints.Sort(SortBySpawnOrder);
	EnemyPoints.Sort(SortBySpawnOrder);
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	// Spawn des alliés.
	const int32 AllySpawnCount = FMath::Min(TestAllyCount, AllyPoints.Num());
	
	 for (int32 i = 0; i < AllySpawnCount; ++i)
    {
        if (!TestAllyClass)
        {
            UE_LOG(LogTemp, Error,
                TEXT("[Spawn] TestAllyClass is not assigned."));
            break;
        }

        AEonBound_PlayerCharacter* Ally = World->SpawnActor<AEonBound_PlayerCharacter>(TestAllyClass,AllyPoints[i]->GetActorTransform(),SpawnParams);

        if (!IsValid(Ally))
        {
            UE_LOG(LogTemp, Error,TEXT("[Spawn] Failed to spawn ally at point %d."),i);
            continue;
        }

        CombatManager->PlayerCombatants.Add(Ally);

        UE_LOG(LogTemp, Log,TEXT("[Spawn] Ally %s spawned at point %d."),*Ally->GetName(), i);
    }

    // Spawn des ennemis.
    const int32 EnemySpawnCount = FMath::Min(TestEnemyCount, EnemyPoints.Num());

    for (int32 i = 0; i < EnemySpawnCount; ++i)
    {
        if (!TestEnemyClass)
        {
            UE_LOG(LogTemp, Error,TEXT("[Spawn] TestEnemyClass is not assigned."));
            break;
        }

        AEonBound_EnemyCharacter* Enemy =
            World->SpawnActor<AEonBound_EnemyCharacter>(TestEnemyClass,EnemyPoints[i]->GetActorTransform(),SpawnParams);

        if (!IsValid(Enemy))
        {
            UE_LOG(LogTemp, Error,TEXT("[Spawn] Failed to spawn enemy at point %d."),i);
            continue;
        }

        Enemy->SetTargetIndicationVisibility(false);
        CombatManager->EnemyCombatants.Add(Enemy);

        UE_LOG(LogTemp, Log,TEXT("[Spawn] Enemy %s spawned at point %d."),*Enemy->GetName(), i);
    }

    UE_LOG(LogTemp, Log,
        TEXT("[Spawn] Result: %d/%d allies, %d/%d enemies."),
			CombatManager->PlayerCombatants.Num(),TestAllyCount,CombatManager->EnemyCombatants.Num(),TestEnemyCount);
}
