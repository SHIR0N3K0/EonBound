// Fill out your copyright notice in the Description page of Project Settings.


#include "EonBound_CombatManager.h"
#include "Engine/Engine.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "EonBound/Character/EonBound_Character.h"
#include "EonBound/GAS/EonBound_AttributeSet.h"
#include "EonBound/UI/CombatHUD_UserWidget.h"
#include "GameFramework/PlayerController.h"


UEonBound_CombatManager::UEonBound_CombatManager()
{
}

void UEonBound_CombatManager::StartCombat()
{	
	UE_LOG(LogTemp, Error, TEXT("Combat started with %d player combatants and %d enemy combatants."), PlayerCombatants.Num(), EnemyCombatants.Num());
	
	// Réinitialise le combat afin de pouvoir démarrer depuis un état connu.
	ClearCombatTimers();

	SetCombatState(EEonBound_CombatState::Preparing);

	ActiveCombatant = nullptr;
	SelectedTarget = nullptr;

	CurrentTurnIndex = 0;
	RoundNumber = 1;

	TargetingMode = ECombatTargetingMode::TargetEnemy;

	TurnOrder.Reset();

	// Un combat ne peut pas commencer si l'une des deux équipes est vide.
	if (!ValidateCombatants())
	{
		UE_LOG(LogTemp, Error, TEXT("[Combat] Invalid combatants."));
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
	// ici, au moment où le combat commence
	for (AEonBound_Character* Combatant : PlayerCombatants)
	{
		if (AEonBound_Character* Character = Cast<AEonBound_Character>(Combatant))
		{
			Character->SetCombatManager(this);
		}
	}

	for (AEonBound_Character* Combatant : EnemyCombatants)
	{
		if (AEonBound_Character* Character = Cast<AEonBound_Character>(Combatant))
		{
			Character->SetCombatManager(this);
		}
	}
	
	// Création du HUD de combat
	if (!IsValid(CombatHUD))
	{
		if (!CombatHUDClass)
		{
			UE_LOG(LogTemp, Error, TEXT("CombatHUDClass is not assigned in game mode."));
			return;
		}

		UWorld* World = GetWorld();

		if (!IsValid(World))
		{
			UE_LOG(LogTemp, Error, TEXT("CombatManager has no valid World."));
			return;
		}

		APlayerController* PlayerController = World->GetFirstPlayerController();

		if (!IsValid(PlayerController))
		{
			UE_LOG(LogTemp, Error, TEXT("No valid PlayerController."));
			return;
		}

		CombatHUD = CreateWidget<UCombatHUD_UserWidget>(PlayerController,CombatHUDClass);

		if (!IsValid(CombatHUD))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create CombatHUD."));
			return;
		}

		CombatHUD->AddToViewport();
	}

	// Création et initialisation d'un statut par personnage allié
	CombatHUD->InitializePlayerStatuses(PlayerCombatants);
	CombatHUD->CombatManagerRef = this;
	
	BuildTurnOrder();

	if (TurnOrder.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("No valid combatants in TurnOrder."));
		return;
	}

	CurrentTurnIndex = 0;
	RoundNumber = 1;

	SetCombatState(EEonBound_CombatState::WaitingForTurn);
	StartCurrentTurn(true);
}

void UEonBound_CombatManager::RequestPlayerBasicAttack(AEonBound_Character* AttackTarget)
{
	UE_LOG(LogTemp, Warning,
		TEXT("[Combat] RequestPlayerBasicAttack called. State: %s | Active: %s | Target: %s | Enemies: %d"),
			*UEnum::GetValueAsString(CombatState),*GetNameSafe(ActiveCombatant),*GetNameSafe(AttackTarget),
				EnemyCombatants.Num());

	if (!PlayerCombatants.Contains(ActiveCombatant))
	{
		return;
	}
	
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

	if (!IsValid(AttackTarget))
	{
		UE_LOG(LogTemp, Error, TEXT("[Combat] Attack blocked: Target is invalid."));
		return;
	}

	if (!IsCombatantAlive(AttackTarget))
	{
		UE_LOG(LogTemp, Error, TEXT("[Combat] Attack blocked: Target is dead or has no valid GAS health."));
		return;
	}
	
	// Vérifie que la cible est bien un ennemi.
	if (!EnemyCombatants.Contains(AttackTarget))
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
	
	Attacker->SetCurrentActionTarget(AttackTarget);
	
	if (!Attacker->TryBasicAttack())
	{
		UE_LOG(LogTemp,Error,TEXT("[Combat] Basic attack activation failed. Returning to PlayerTurn."));
		Attacker->SetCurrentActionTarget(nullptr);
		SetCombatState(EEonBound_CombatState::PlayerTurn);
		return;
	}
	
	AttackTarget->SetTargetIndicationVisibility(false);
	
	FString AttackerName = Attacker->GetName();
	UE_LOG(LogTemp, Error, TEXT("The attacker is %s"), *AttackerName);
	
	if (CombatHUD)
	{
		CombatHUD->SetButtonVisibility(ECombatHUDState::Basic,false);
		CombatHUD->SetButtonVisibility(ECombatHUDState::Skill,false);
	}

	TargetingMode = ECombatTargetingMode::TargetEnemy;
}

void UEonBound_CombatManager::RequestPlayerSkill(AEonBound_Character* SkillTarget)
{
	if (CombatState != EEonBound_CombatState::PlayerTurn)
	{
		UE_LOG(LogTemp, Error, TEXT("[Combat] Skill request blocked: not the player's turn."));
		return;
	}

	AEonBound_Character* SkillUser = ActiveCombatant;

	if (CombatState != EEonBound_CombatState::PlayerTurn)
	{
		return;
	}

	if (!IsValid(SkillUser) || !PlayerCombatants.Contains(SkillUser))
	{
		return;
	}

	bool bValidTarget = false;

	switch (TargetingMode)
	{
	case ECombatTargetingMode::TargetAlly:

		bValidTarget = PlayerCombatants.Contains(SkillTarget);
		break;

	case ECombatTargetingMode::TargetEnemy:

		bValidTarget = EnemyCombatants.Contains(SkillTarget);
		break;

	default:
		return;
	}

	if (!bValidTarget)
	{
		return;
	}

	// Vérifier que la cible est vivante.
	if (!IsCombatantAlive(SkillTarget))
	{
		return;
	}

	// Transmettre la cible.
	SkillUser->SetCurrentActionTarget(SkillTarget);

	// Quitter le mode de ciblage.
	TargetingMode = ECombatTargetingMode::TargetNone;

	// Activer la compétence.
	SetCombatState(EEonBound_CombatState::ExecutingAction);

	if (!SkillUser->TrySkill())
	{
		UE_LOG(LogTemp, Error,
			TEXT("[Combat] Skill activation failed."));

		SetCombatState(EEonBound_CombatState::PlayerTurn);
		return;
	}

	// Masquer les boutons d'action.
	if (CombatHUD)
	{
		CombatHUD->SetButtonVisibility(ECombatHUDState::Basic, false);
		CombatHUD->SetButtonVisibility(ECombatHUDState::Skill, false);
	}

	SkillTarget->SetTargetIndicationVisibility(false);
	
	TargetingMode = ECombatTargetingMode::TargetEnemy;
	
	UE_LOG(LogTemp, Warning,
	TEXT("[Combat] Skill request | CombatState: %d | Target: %s"),
	static_cast<int32>(CombatState),
	*GetNameSafe(SkillTarget));
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
	
	// Si la cible est morte, choisir un autre ennemi vivant.
	if (!IsValid(SelectedTarget) || !IsCombatantAlive(SelectedTarget))
	{
		SelectFirstLivingEnemy();
	}

	// Le tour suivant est préparé.	
	AdvanceTurn();
}

void UEonBound_CombatManager::CheckCombatEnd()
{
	bool bAnyPlayerAlive = false;
	bool bAnyEnemyAlive = false;

	if (CombatState == EEonBound_CombatState::Victory || CombatState == EEonBound_CombatState::Defeat)
	{
		return;
	}
	
	// Vérifie si au moins un combattant de chaque équipe est encore vivant.
	for (AEonBound_Character* Player : PlayerCombatants)
	{
		if (IsCombatantAlive(Player))
		{
			bAnyPlayerAlive = true;
			break;
		}
	}

	for (AEonBound_Character* Enemy : EnemyCombatants)
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
		ClearCombatTimers();
		
		SetCombatState(EEonBound_CombatState::Victory);
		ActiveCombatant = nullptr;

		UE_LOG(LogTemp, Warning, TEXT("VICTORY"));
		return;
	}

	if (!bAnyPlayerAlive)
	{
		ClearCombatTimers();
		
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

void UEonBound_CombatManager::SetSelectedTarget(AEonBound_Character* RequestedTarget)
{
	if (CombatState != EEonBound_CombatState::PlayerTurn)
	{
		return;
	}
	
	// Si la cible demandée est invalide, morte ou n'est pas un ennemi,
	// conserver la sélection actuelle si elle est encore valide.
	if (!IsValid(RequestedTarget) || !EnemyCombatants.Contains(RequestedTarget) || !IsCombatantAlive(RequestedTarget))
	{
		if (IsValid(SelectedTarget) && EnemyCombatants.Contains(SelectedTarget) && IsCombatantAlive(SelectedTarget))
		{
			return;
		}

		// La sélection actuelle n'est plus valide : trouver un remplaçant.
		SelectFirstLivingEnemy();
		return;
	}
		
		// Ne rien faire si la cible est déjà sélectionnée.
	if (SelectedTarget == RequestedTarget)
	{
		return;
	}
		
		// Met à jour la visibilité de l'indication de ciblage pour l'ancienne cible.
	if (IsValid(SelectedTarget))
	{
		SelectedTarget->SetTargetIndicationVisibility(false);
	}

		// Met à jour la cible actuelle.
	SelectedTarget = RequestedTarget;
	SelectedTarget->SetTargetIndicationVisibility(true);
}

void UEonBound_CombatManager::BeginTargeting(ECombatTargetingMode NewTargetingMode)
{
	if (CombatState != EEonBound_CombatState::PlayerTurn)
	{
		return;
	}

	if (!IsValid(ActiveCombatant))
	{
		return;
	}

	TargetingMode = NewTargetingMode;
	UE_LOG(LogTemp, Warning,TEXT("[Combat] Targeting mode set to: %s"),
		TargetingMode == ECombatTargetingMode::TargetAlly? TEXT("TargetAlly"): TEXT("TargetEnemy"));

	// Masquer l'indication de l'ancien ennemi sélectionné.
	if (IsValid(SelectedTarget))
	{
		SelectedTarget->SetTargetIndicationVisibility(false);
	}

	UE_LOG(LogTemp, Warning,TEXT("[Combat] Entered Targeting Mode."));
}

void UEonBound_CombatManager::CancelTargeting()
{
	if (CombatState != EEonBound_CombatState::PlayerTurn)
	{
		return;
	}

	TargetingMode = ECombatTargetingMode::TargetEnemy;

	if (IsValid(SelectedTarget) && IsCombatantAlive(SelectedTarget))
	{
		SelectedTarget->SetTargetIndicationVisibility(true);
	}
}

void UEonBound_CombatManager::ToggleSkillTargeting()
{
	if (CombatState != EEonBound_CombatState::PlayerTurn)
	{
		return;
	}

	// Vérifier qu'un personnage actif existe.
	if (!IsValid(ActiveCombatant))
	{
		return;
	}

	AEonBound_Character* ActiveCharacter =
		Cast<AEonBound_Character>(ActiveCombatant);

	if (!ActiveCharacter)
	{
		return;
	}

	// Basculer entre BasicAttack et Heal.
	if (TargetingMode == ECombatTargetingMode::TargetEnemy)
	{
		TargetingMode = ECombatTargetingMode::TargetAlly;
	}
	else
	{
		TargetingMode = ECombatTargetingMode::TargetEnemy;
	}

	// Réinitialiser l'indication de sélection précédente.
	if (IsValid(SelectedTarget))
	{
		SelectedTarget->SetTargetIndicationVisibility(false);
	}

	UE_LOG(LogTemp, Warning,TEXT("[Combat] Targeting mode changed to: %s"),
		TargetingMode == ECombatTargetingMode::TargetAlly? TEXT("TargetAlly"): TEXT("TargetEnemy"));
}

void UEonBound_CombatManager::SetCombatState(EEonBound_CombatState NewState)
{
	// Centralise la mise à jour de l'état pour conserver des logs cohérents.
	CombatState = NewState;
	
	UE_LOG(LogTemp, Warning,TEXT("Combat state: %s"),*UEnum::GetValueAsString(CombatState));
}

void UEonBound_CombatManager::StartEnemyTurn()
{
	// Vérifie que c'est bien le tour de l'ennemi et qu'un combattant actif est défini.
	if (CombatState != EEonBound_CombatState::EnemyTurn || !IsValid(ActiveCombatant))
	{
		return;
	}
	
	AEonBound_Character* Target = nullptr;

	// Sélectionne le premier joueur encore vivant comme cible de l'attaque.
	for (AEonBound_Character* Player : PlayerCombatants)
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

	Attacker->SetCurrentActionTarget(Target);
	SetCombatState(EEonBound_CombatState::ExecutingAction);
	
	if (!Attacker->TryBasicAttack())
	{
		Attacker->SetCurrentActionTarget(nullptr);
		UE_LOG(LogTemp,Error,TEXT("[Combat] Basic attack activation failed. Returning to EnemyTurn."));

		AdvanceTurn();
		return;
	}
	
	FString AttackerName = Attacker->GetName();
	UE_LOG(LogTemp, Error, TEXT("The attacker is %s"), *AttackerName);
}

bool UEonBound_CombatManager::IsCombatantAlive(AEonBound_Character* Combatant) const
{
	// Vérifie que le combattant est valide et possède un composant GAS.
	if (!IsValid(Combatant) || Combatant->IsDead())
	{
		return false;
	}

	UAbilitySystemComponent* ASC = Combatant->GetAbilitySystemComponent();

	if (!ASC)
	{
		return false;
	}
	
	// Vérifie que le combattant possède l'attribut de santé et qu'il est supérieur à zéro.
	const FGameplayAttribute HealthAttribute = UEonBound_AttributeSet::GetHealthAttribute();

	return ASC->GetNumericAttribute(HealthAttribute) > 0.f;
}

void UEonBound_CombatManager::BuildTurnOrder()
{
	TurnOrder.Reset();
	
	// Ajouter les combattants des deux équipes.
	for (AEonBound_Character* Combatant : PlayerCombatants)
	{
		if (IsCombatantAlive(Combatant))
		{
			TurnOrder.Add(Combatant);
		}
	}
	
	for (AEonBound_Character* Combatant : EnemyCombatants)
	{
		if (IsCombatantAlive(Combatant))
		{
			TurnOrder.Add(Combatant);
		}
	}
	
	// Trier par Speed décroissante.
	// En cas d'égalité, l'ordre précédent est conservé.	
	TurnOrder.StableSort([this](const AEonBound_Character& A, const AEonBound_Character& B)
	{
	return GetCombatantSpeed(&A) > GetCombatantSpeed(&B);
	});

	CurrentTurnIndex = 0;
	RoundNumber = 1;
	
	UE_LOG(LogTemp, Error, TEXT("The current order of play is:"));

	for (int32 i = 0; i < TurnOrder.Num(); i++)
	{
		
		AEonBound_Character* Actor = TurnOrder[i].Get();AEonBound_Character* Character = Cast<AEonBound_Character>(Actor);
		if (!Character)
		{
			continue;
		}
		
		if (TurnOrder[i])
		{
			UE_LOG(LogTemp, Error, TEXT("TurnOrder[%d]: %s"),i,*TurnOrder[i]->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("TurnOrder[%d]: nullptr"), i);
		}
		UE_LOG(LogTemp,Warning,TEXT("Initiative [%d] : %s | Speed = %.2f"),i,*Character->GetName(),GetCombatantSpeed(Character));
	}
}

float UEonBound_CombatManager::GetCombatantSpeed(const AEonBound_Character* Combatant) const
{
	if (!IsValid(Combatant))
	{
		return 0.f;
	}

	UAbilitySystemComponent* ASC = Combatant->GetAbilitySystemComponent();

	if (!ASC)
	{
		return 0.f;
	}

	return ASC->GetNumericAttribute(UEonBound_AttributeSet::GetSpeedAttribute());
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

		AEonBound_Character* NextCombatant = TurnOrder[CurrentTurnIndex].Get();

		if (IsCombatantAlive(NextCombatant))
		{
			SetCombatState(EEonBound_CombatState::WaitingForTurn);
			StartCurrentTurn(false);
			return;
		}
	}

	// Aucun combattant ne peut agir.
	CheckCombatEnd();
}

void UEonBound_CombatManager::StartCurrentTurn(bool bIsNewRound)
{
	if (TurnOrder.IsEmpty())
	{
		return;
	}

	if (!TurnOrder.IsValidIndex(CurrentTurnIndex))
	{
		return;
	}

	AEonBound_Character* Combatant = TurnOrder[CurrentTurnIndex].Get();

	// Ignorer les combattants morts ou invalides.
	if (!IsCombatantAlive(Combatant))
	{
		AdvanceTurn();
		return;
	}

	ActiveCombatant = Combatant;
	
	// Avancer les cooldowns des compétences du combattant actif.
	UEonBound_AbilitySystemComponent* ASC = Cast<UEonBound_AbilitySystemComponent>(Combatant->GetAbilitySystemComponent());
	if (ASC)
	{
		ASC->AdvanceTurnCooldowns();
	}
	
	// Déterminer l'équipe du combattant.
	if (PlayerCombatants.Contains(Combatant))
	{
		SetCombatState(EEonBound_CombatState::PlayerTurn);
		TargetingMode = ECombatTargetingMode::TargetEnemy;
		
		if (CombatHUD)
		{
			CombatHUD->SetButtonVisibility(ECombatHUDState::Basic,true);
			//CombatHUD->SetButtonVisibility(ECombatHUDState::Skill,true);
		}

		if (bIsNewRound or IsCombatantAlive(SelectedTarget) == false)
		{
			// Préparer la cible par défaut du joueur.
			SelectedTarget = nullptr;

			for (AEonBound_Character* Enemy : EnemyCombatants)
			{
				if (IsCombatantAlive(Enemy))
				{
					SetSelectedTarget(Enemy);
					break;
				}
			}
		}
		else
		{
			SelectedTarget->SetTargetIndicationVisibility(true);
		}

		UE_LOG(LogTemp, Warning,TEXT("[Turn] Player turn: %s | Round: %d"),*GetNameSafe(ActiveCombatant),RoundNumber);
	}
	else if (EnemyCombatants.Contains(Combatant))
	{
		SetCombatState(EEonBound_CombatState::EnemyTurn);
		
		if (IsValid(SelectedTarget))
		{
			SelectedTarget->SetTargetIndicationVisibility(false);
		}

		if (bIsNewRound)
		{
			GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle,this, &UEonBound_CombatManager::OnDelayTimerFinished,2.0f,false);
		}
		else
		{
			UE_LOG(LogTemp, Warning,TEXT("[Turn] Enemy turn: %s | Round: %d"),*GetNameSafe(ActiveCombatant),RoundNumber);

			StartEnemyTurn();
		}
	}
}

void UEonBound_CombatManager::SelectFirstLivingEnemy()
{
	// Masquer l'indicateur de la cible précédente.
	if (IsValid(SelectedTarget))
	{
		SelectedTarget->SetTargetIndicationVisibility(false);
	}

	SelectedTarget = nullptr;

	// Sélectionner le premier ennemi vivant.
	for (AEonBound_Character* Enemy : EnemyCombatants)
	{
		if (IsValid(Enemy) && IsCombatantAlive(Enemy))
		{
			SelectedTarget = Enemy;
			SelectedTarget->SetTargetIndicationVisibility(true);
			return;
		}
	}
}

bool UEonBound_CombatManager::ValidateCombatants()
{
	if (PlayerCombatants.IsEmpty() || EnemyCombatants.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("[Combat] Invalid combatants."));
		return false;
	}
	for (AEonBound_Character* Player : PlayerCombatants)
	{
		if (!IsValid(Player))
		{
			if (IsCombatantAlive(Player) && !Player->IsDead() && Player->GetAbilitySystemComponent())
			{
				UE_LOG(LogTemp, Error, TEXT("[Combat] Invalid player combatant."));
				return false;
			}
			UE_LOG(LogTemp, Error, TEXT("[Combat] Invalid player combatant."));
			return false;
		}
	}
	for (AEonBound_Character* Enemy : EnemyCombatants)
	{
		if (!IsValid(Enemy))
		{
			if (IsCombatantAlive(Enemy) && !Enemy->IsDead() && Enemy->GetAbilitySystemComponent())
			{
				UE_LOG(LogTemp, Error, TEXT("[Combat] Invalid enemy combatant."));
				return false;
			}
			UE_LOG(LogTemp, Error, TEXT("[Combat] Invalid enemy combatant."));
			return false;
		}
	}
	return true;
}

void UEonBound_CombatManager::OnDelayTimerFinished()
{
	UE_LOG(LogTemp, Warning,TEXT("[Turn] Enemy turn: %s | Round: %d"),*GetNameSafe(ActiveCombatant),RoundNumber);

	if (CombatState != EEonBound_CombatState::EnemyTurn)
	{
		return;
	}

	if (!IsValid(ActiveCombatant))
	{
		return;
	}
	
	StartEnemyTurn();
}

void UEonBound_CombatManager::ClearCombatTimers()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(DelayTimerHandle);
	}
}
