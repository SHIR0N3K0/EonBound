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
	
	BuildTurnOrder();

	if (TurnOrder.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("No valid combatants in TurnOrder."));
		return;
	}

	CurrentTurnIndex = 0;
	RoundNumber = 1;

	SetCombatState(EEonBound_CombatState::WaitingForTurn);
	StartCurrentTurn();
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

	// La cible est transmise au personnage attaquant.
	AEonBound_Character* Attacker = Cast<AEonBound_Character>(ActiveCombatant);
	
	if (!Attacker)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[Combat] Attack blocked: ActiveCombatant is not an AEonBound_Character."));
		SetCombatState(EEonBound_CombatState::PlayerTurn);
		return;
	}

	SetCombatState(EEonBound_CombatState::ExecutingAction);
	
	Attacker->CurrentTarget = Target;
	
	if (!Attacker->TryBasicAttack())
	{
		UE_LOG(LogTemp,Error,TEXT("[Combat] Basic attack activation failed. Returning to PlayerTurn."));

		SetCombatState(EEonBound_CombatState::PlayerTurn);
	}
	
	FString AttackerName = Attacker->GetName();
	UE_LOG(LogTemp, Error, TEXT("The attacker is %s"), *AttackerName);

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

	// Le tour suivant est préparé.	
	AdvanceTurn();
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
	
	FString AttackerName = Attacker->GetName();
	UE_LOG(LogTemp, Error, TEXT("The attacker is %s"), *AttackerName);
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

void UEonBound_CombatManager::BuildTurnOrder()
{
	TurnOrder.Reset();
	
	// Ajouter les combattants des deux équipes.
	for (AActor* Combatant : PlayerCombatants)
	{
		if (IsCombatantAlive(Combatant))
		{
			TurnOrder.Add(Combatant);
		}
	}
	
	for (AActor* Combatant : EnemyCombatants)
	{
		if (IsCombatantAlive(Combatant))
		{
			TurnOrder.Add(Combatant);
		}
	}
	
	// Trier par Speed décroissante.
	// En cas d'égalité, l'ordre précédent est conservé.	
	TurnOrder.StableSort([this](const AActor& A, const AActor& B)
	{
		const AEonBound_Character* CharacterA = Cast<AEonBound_Character>(&A);

		const AEonBound_Character* CharacterB = Cast<AEonBound_Character>(&B);

		return GetCombatantSpeed(const_cast<AEonBound_Character*>(CharacterA)) > GetCombatantSpeed(const_cast<AEonBound_Character*>(CharacterB));
	});

	CurrentTurnIndex = 0;
	RoundNumber = 1;
	
	UE_LOG(LogTemp, Error, TEXT("The current order of play is:"));

	for (int32 i = 0; i < TurnOrder.Num(); i++)
	{
		
		AActor* Actor = TurnOrder[i].Get();AEonBound_Character* Character = Cast<AEonBound_Character>(Actor);
		if (!Character)
		{
			continue;
		}
		
		if (TurnOrder[i])
		{
			UE_LOG(LogTemp, Error, TEXT("TurnOrder[%d]: %s"),
				i,
				*TurnOrder[i]->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("TurnOrder[%d]: nullptr"), i);
		}
		UE_LOG(LogTemp,Warning,TEXT("Initiative [%d] : %s | Speed = %.2f"),i,*Character->GetName(),GetCombatantSpeed(Character));
	}
}

float UEonBound_CombatManager::GetCombatantSpeed(AEonBound_Character* Combatant) const
{
	if (!IsValid(Combatant))
	{
		return 0.f;
	}

	IAbilitySystemInterface* AbilitySystemInterface =
		Cast<IAbilitySystemInterface>(Combatant);

	if (!AbilitySystemInterface)
	{
		return 0.f;
	}

	UAbilitySystemComponent* ASC =
		AbilitySystemInterface->GetAbilitySystemComponent();

	if (!ASC)
	{
		return 0.f;
	}

	return ASC->GetNumericAttribute(
		UEonBound_AttributeSet::GetSpeedAttribute()
	);
}

void UEonBound_CombatManager::AdvanceTurn()
{
	CheckCombatEnd();

	if (CombatState == EEonBound_CombatState::Victory ||
		CombatState == EEonBound_CombatState::Defeat)
	{
		return;
	}

	if (TurnOrder.IsEmpty())
	{
		return;
	}

	// Chercher le prochain combattant vivant.
	for (int32 Attempts = 0;
		 Attempts < TurnOrder.Num();
		 ++Attempts)
	{
		++CurrentTurnIndex;

		// Fin du cycle : recommencer au début.
		if (CurrentTurnIndex >= TurnOrder.Num())
		{
			CurrentTurnIndex = 0;
			++RoundNumber;

			UE_LOG(LogTemp, Warning,TEXT("[Turn] Starting round %d"),RoundNumber);
		}

		AActor* NextCombatant = TurnOrder[CurrentTurnIndex].Get();

		if (IsCombatantAlive(NextCombatant))
		{
			SetCombatState(EEonBound_CombatState::WaitingForTurn);
			StartCurrentTurn();
			return;
		}
	}

	// Aucun combattant ne peut agir.
	CheckCombatEnd();
}

void UEonBound_CombatManager::StartCurrentTurn()
{
	if (TurnOrder.IsEmpty())
	{
		return;
	}

	if (!TurnOrder.IsValidIndex(CurrentTurnIndex))
	{
		return;
	}

	AActor* Combatant = TurnOrder[CurrentTurnIndex].Get();

	// Ignorer les combattants morts ou invalides.
	if (!IsCombatantAlive(Combatant))
	{
		AdvanceTurn();
		return;
	}

	ActiveCombatant = Combatant;

	// Déterminer l'équipe du combattant.
	if (PlayerCombatants.Contains(Combatant))
	{
		SetCombatState(EEonBound_CombatState::PlayerTurn);

		// Préparer la cible par défaut du joueur.
		PlayerTarget = nullptr;

		for (AActor* Enemy : EnemyCombatants)
		{
			if (IsCombatantAlive(Enemy))
			{
				PlayerTarget = Enemy;
				break;
			}
		}

		UE_LOG(LogTemp, Warning,TEXT("[Turn] Player turn: %s | Round: %d"),*GetNameSafe(ActiveCombatant),RoundNumber);
	}
	else if (EnemyCombatants.Contains(Combatant))
	{
		SetCombatState(EEonBound_CombatState::EnemyTurn);

		UE_LOG(LogTemp, Warning,TEXT("[Turn] Enemy turn: %s | Round: %d"),*GetNameSafe(ActiveCombatant),RoundNumber);

		StartEnemyTurn();
	}
}
