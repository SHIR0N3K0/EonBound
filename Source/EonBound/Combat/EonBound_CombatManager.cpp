// Fill out your copyright notice in the Description page of Project Settings.


#include "EonBound_CombatManager.h"
#include "Engine/Engine.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "EonBound/Character/EonBound_Character.h"
#include "EonBound/GAS/EonBound_AttributeSet.h"


UEonBound_CombatManager::UEonBound_CombatManager()
{
}

void UEonBound_CombatManager::StartCombat()
{	
	UE_LOG(LogTemp, Error, TEXT("Combat started with %d player combatants and %d enemy combatants."), PlayerCombatants.Num(), EnemyCombatants.Num());
	// Réinitialise le combat afin de pouvoir démarrer depuis un état connu.
	SetCombatState(EEonBound_CombatState::Preparing);
	ActiveCombatant = nullptr;

	// Un combat ne peut pas commencer si l'une des deux équipes est vide.
	if (PlayerCombatants.IsEmpty() || EnemyCombatants.IsEmpty())
	{
		UE_LOG(LogTemp, Error,TEXT("Combat impossible : équipe vide."));
		return;
	}

	// Vérifie immédiatement si le combat est déjà terminé
	// avant de sélectionner le premier combattant.
	CheckCombatEnd();

	if (CombatState == EEonBound_CombatState::Victory || CombatState == EEonBound_CombatState::Defeat)
	{
		return;
	}

	// Le manager est créé en C++ : chaque personnage reçoit donc sa référence
	// ici, au moment où le combat commence, plutôt que via le Blueprint.
	for (AActor* Combatant : PlayerCombatants)
	{
		if (AEonBound_Character* Character = Cast<AEonBound_Character>(Combatant))
		{
			Character->SetCombatManager(this);
		}
	}

	for (AActor* Combatant : EnemyCombatants)
	{
		if (AEonBound_Character* Character = Cast<AEonBound_Character>(Combatant))
		{
			Character->SetCombatManager(this);
		}
	}
	
	// Le premier combattant disponible sera sélectionné par SelectNextCombatant().
	SetCombatState(EEonBound_CombatState::WaitingForTurn);
	SelectNextCombatant();
}

void UEonBound_CombatManager::RequestPlayerBasicAttack(AActor* Target)
{
	UE_LOG(LogTemp, Warning,
		TEXT("[Combat] RequestPlayerBasicAttack called. State: %s | Active: %s | Target: %s | Enemies: %d"),
		*UEnum::GetValueAsString(CombatState),
		*GetNameSafe(ActiveCombatant),
		*GetNameSafe(Target),
		EnemyCombatants.Num());

	// Vérifie que le combat est en cours et que c'est bien le tour du joueur.
	if (CombatState != EEonBound_CombatState::PlayerTurn)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[Combat] Attack blocked: expected PlayerTurn, current state is %s."),
			*UEnum::GetValueAsString(CombatState));
		return;
	}
	
	// Vérifie que le combattant actif et la cible sont valides et vivants.
	if (!IsValid(ActiveCombatant))
	{
		UE_LOG(LogTemp, Error, TEXT("[Combat] Attack blocked: ActiveCombatant is invalid."));
		return;
	}

	if (!IsValid(Target))
	{
		UE_LOG(LogTemp, Error, TEXT("[Combat] Attack blocked: Target is invalid."));
		return;
	}

	if (!IsCombatantAlive(Target))
	{
		UE_LOG(LogTemp, Error, TEXT("[Combat] Attack blocked: Target is dead or has no valid GAS health."));
		return;
	}
	
	// Vérifie que la cible est bien un ennemi.
	if (!EnemyCombatants.Contains(Target))
	{
		UE_LOG(LogTemp, Error, TEXT("[Combat] Attack blocked: Target is not in EnemyCombatants."));
		return;
	}
	
	// L'action commence.
	SetCombatState(EEonBound_CombatState::ExecutingAction);

	// La cible est transmise au personnage attaquant.
	AEonBound_Character* Attacker = Cast<AEonBound_Character>(ActiveCombatant);
	
	if (!Attacker)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[Combat] Attack blocked: ActiveCombatant is not an AEonBound_Character."));
		SetCombatState(EEonBound_CombatState::PlayerTurn);
		return;
	}

	Attacker->CurrentTarget = Target;
	Attacker->TryBasicAttack();

}

void UEonBound_CombatManager::NotifyActionFinished()
{
	// Vérifie que le combat est en cours et qu'une action était en train de s'exécuter.
	if (CombatState != EEonBound_CombatState::ExecutingAction)
	{
		return;
	}

	CheckCombatEnd();
	
	// Si le combat est terminé, on ne passe pas au tour suivant.
	if (CombatState == EEonBound_CombatState::Victory || CombatState == EEonBound_CombatState::Defeat)
	{
		return;
	}

	// Inverse le tour pour que l'équipe adverse joue ensuite.
	bNextTurnIsPlayer = !bNextTurnIsPlayer;

	// Le tour suivant est préparé.
	SetCombatState(EEonBound_CombatState::WaitingForTurn);
	SelectNextCombatant();
}

void UEonBound_CombatManager::CheckCombatEnd()
{
	bool bAnyPlayerAlive = false;
	bool bAnyEnemyAlive = false;

	// Vérifie si au moins un combattant de chaque équipe est encore vivant.
	for (AActor* Player : PlayerCombatants)
	{
		if (IsCombatantAlive(Player))
		{
			bAnyPlayerAlive = true;
			break;
		}
	}

	for (AActor* Enemy : EnemyCombatants)
	{
		if (IsCombatantAlive(Enemy))
		{
			bAnyEnemyAlive = true;
			break;
		}
	}

	// Si l'une des équipes est entièrement éliminée, le combat se termine.
	if (!bAnyEnemyAlive)
	{
		SetCombatState(EEonBound_CombatState::Victory);
		ActiveCombatant = nullptr;

		UE_LOG(LogTemp, Warning, TEXT("VICTORY"));
		return;
	}

	if (!bAnyPlayerAlive)
	{
		SetCombatState(EEonBound_CombatState::Defeat);
		ActiveCombatant = nullptr;

		UE_LOG(LogTemp, Warning, TEXT("DEFEAT"));
		return;
	}
}

EEonBound_CombatState UEonBound_CombatManager::GetCombatState() const
{
	return CombatState;
}

void UEonBound_CombatManager::SetCombatState(EEonBound_CombatState NewState)
{
	// Centralise la mise à jour de l'état pour conserver des logs cohérents.
	CombatState = NewState;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf
			(TEXT("Combat state changed to: %s"), *UEnum::GetValueAsString(CombatState)));
	}
	UE_LOG(LogTemp, Warning,TEXT("Combat state: %s"),*UEnum::GetValueAsString(CombatState));
}

void UEonBound_CombatManager::SelectNextCombatant()
{
	// Ne sélectionne pas de nouveau combattant une fois le combat terminé.
	if (CombatState == EEonBound_CombatState::Victory || CombatState == EEonBound_CombatState::Defeat)
	{
		return;
	}
	
	// Met à jour l'état du combat avant de chercher le prochain tour.
	CheckCombatEnd();
	
	// L'équipe interrogée dépend de qui à jouer précedement : joueur / ennemis.
	TArray<TObjectPtr<AActor>>& Team = bNextTurnIsPlayer ? PlayerCombatants : EnemyCombatants;
	
	ActiveCombatant = nullptr;
		
	// Sélectionne le premier combattant encore disponible dans l'équipe.
	for (AActor* Combatant : Team)
	{
		if (IsCombatantAlive(Combatant))
		{
			ActiveCombatant = Combatant;
			break;
		}
	}
	
	// Si tous les combattants de l'équipe attendue
	// sont morts, vérifier la fin du combat.
	if (!ActiveCombatant)
	{
		// L'équipe prévue n'a plus de combattant actif.
		// On vérifie d'abord si cela termine le combat.
		CheckCombatEnd();

		if (CombatState == EEonBound_CombatState::Victory || CombatState == EEonBound_CombatState::Defeat)
		{
			return;
		}
		
		// Sinon, l'équipe adverse prend le prochain tour.
		bNextTurnIsPlayer = !bNextTurnIsPlayer;
		SelectNextCombatant();
		return;
	}
	
	// Informe le reste du jeu du type de tour qui vient de commencer.
	if (bNextTurnIsPlayer)
	{
		SetCombatState(EEonBound_CombatState::PlayerTurn);
		for (AActor* Enemy : EnemyCombatants)
		{
			if (IsCombatantAlive(Enemy))
			{
				PlayerTarget = Enemy;
				break;
			}
		}
	}
	else
	{
		SetCombatState(EEonBound_CombatState::EnemyTurn);
		StartEnemyTurn();
	}
}

void UEonBound_CombatManager::StartEnemyTurn()
{
	// Vérifie que c'est bien le tour de l'ennemi et qu'un combattant actif est défini.
	if (CombatState != EEonBound_CombatState::EnemyTurn || !IsValid(ActiveCombatant))
	{
		return;
	}
	
	AActor* Target = nullptr;

	// Sélectionne le premier joueur encore vivant comme cible de l'attaque.
	for (AActor* Player : PlayerCombatants)
	{
		if (IsCombatantAlive(Player))
		{
			Target = Player;
			break;
		}
	}

	if (!Target)
	{
		CheckCombatEnd();
		return;
	}

	
	AEonBound_Character* Attacker = Cast<AEonBound_Character>(ActiveCombatant);

	if (!Attacker)
	{
		UE_LOG(LogTemp, Error,
			TEXT("Le combattant ennemi n'est pas un Character compatible."));
		return;
	}

	Attacker->CurrentTarget = Target;
	SetCombatState(EEonBound_CombatState::ExecutingAction);
	Attacker->TryBasicAttack();
}

bool UEonBound_CombatManager::IsCombatantAlive(AActor* Combatant) const
{
	// Vérifie que le combattant est valide et possède un composant GAS.
	if (!IsValid(Combatant))
	{
		return false;
	}

	IAbilitySystemInterface* ASI =
		Cast<IAbilitySystemInterface>(Combatant);

	if (!ASI)
	{
		return false;
	}

	UAbilitySystemComponent* ASC =
		ASI->GetAbilitySystemComponent();

	if (!ASC)
	{
		return false;
	}
	
	// Vérifie que le combattant possède l'attribut de santé et qu'il est supérieur à zéro.
	const FGameplayAttribute HealthAttribute =UEonBound_AttributeSet::GetHealthAttribute();

	return ASC->GetNumericAttribute(HealthAttribute) > 0.f;
}
