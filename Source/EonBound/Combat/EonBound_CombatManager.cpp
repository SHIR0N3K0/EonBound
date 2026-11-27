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
	
	EndSkillTargeting();

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
	
	StartGaugeCharging();
	SetCombatState(EEonBound_CombatState::WaitingForTurn);
	StartCurrentTurn(true);
}

void UEonBound_CombatManager::RequestPlayerBasicAttack(AEonBound_Character* AttackTarget)
{
	UE_LOG(LogTemp, Warning,
		TEXT("[Combat] RequestPlayerBasicAttack called. State: %s | Active: %s | Target: %s | Enemies: %d"),
			*UEnum::GetValueAsString(CombatState),*GetNameSafe(ActiveCombatant),*GetNameSafe(AttackTarget),
				EnemyCombatants.Num());

	if (bSkillTargeting)
	{
		CancelTargeting();
		return;
	}
	
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
		CombatHUD->SetButtonVisibility(ECombatHUDState::Ult, false);
	}

	TargetingMode = ECombatTargetingMode::TargetEnemy;
}

void UEonBound_CombatManager::NotifyActionFinished()
{
	// Vérifie que le combat est en cours et qu'une action était en train de s'exécuter.
	if (CombatState != EEonBound_CombatState::ExecutingAction)
	{
		return;
	}

	// Les cibles de la compétence ne servent plus.
	if (IsValid(ActiveCombatant))
	{
		ActiveCombatant->SetCurrentActionTargets(TArray<AEonBound_Character*>());
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
	
	// Pendant le ciblage d'une compétence, un clic choisit une cible de compétence.
	if (bSkillTargeting)
	{
		PickSkillTarget(RequestedTarget);
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
	
	if (bSkillTargeting)
	{
		EndSkillTargeting();
	}

	TargetingMode = ECombatTargetingMode::TargetEnemy;

	if (IsValid(SelectedTarget) && IsCombatantAlive(SelectedTarget))
	{
		SelectedTarget->SetTargetIndicationVisibility(true);
	}
}

void UEonBound_CombatManager::ToggleSkillTargeting()
{
	ToggleSlotTargeting(ECombatSkillSlot::Skill);
}

void UEonBound_CombatManager::ToggleUltimateTargeting()
{
	ToggleSlotTargeting(ECombatSkillSlot::Ultimate);
}

void UEonBound_CombatManager::ToggleSlotTargeting(ECombatSkillSlot Slot)
{
	if (CombatState != EEonBound_CombatState::PlayerTurn || !IsValid(ActiveCombatant))
	{
		return;
	}

	if (bSkillTargeting)
	{
		// Même touche : confirmer. Autre touche : changer d'emplacement.
		if (ActiveSlot == Slot)
		{
			ConfirmSkill();
			return;
		}

		EndSkillTargeting();
	}

	BeginSkillTargeting(Slot);
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
	
	EndSkillTargeting();
	
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
			// Le bouton Skill n'apparaît que si le personnage a une compétence
			// il est grisé tant qu'elle est en cooldown.
			CombatHUD->SetButtonVisibility(ECombatHUDState::Skill, Combatant->GetSkillTargeting() != nullptr);
			// L'overlay Ult n'apparaît que si le personnage a un ultime.
			CombatHUD->SetButtonVisibility(ECombatHUDState::Ult, Combatant->GetUltimateTargeting() != nullptr);
			CombatHUD->SetUltButtonEnabled(Combatant->CanUseUltimate());
			CombatHUD->SetSkillButtonEnabled(Combatant->CanUseSkill());
			CombatHUD->SetSkillCooldown(Combatant->GetSkillCooldownRemaining());
			const float UltCost = Combatant->GetUltimateEnergyCost();
			CombatHUD->SetEnergyDisplay(Combatant->GetEnergy(), UltCost > 0.f ? UltCost : Combatant->GetEnergyMax());
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
		GetWorld()->GetTimerManager().ClearTimer(GaugeTimerHandle);
	}
}

void UEonBound_CombatManager::BeginSkillTargeting(ECombatSkillSlot Slot)
{

	AEonBound_Character* Caster = ActiveCombatant;
	const bool bUltimate = (Slot == ECombatSkillSlot::Ultimate);

	const FSkillTargeting* Targeting = nullptr;
	if (IsValid(Caster))
	{
		Targeting = bUltimate ? Caster->GetUltimateTargeting() : Caster->GetSkillTargeting();
	}

	if (!Targeting)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Combat] %s n'a pas de capacité pour cet emplacement."), *GetNameSafe(Caster));
		return;
	}

	const bool bUsable = bUltimate ? Caster->CanUseUltimate() : Caster->CanUseSkill();

	if (!bUsable)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Combat] Capacité indisponible (cooldown, énergie ou non donnée)."));
		return;
	}

	ActiveSlot = Slot;

	if (!Caster->CanUseSkill())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Combat] Compétence indisponible (cooldown ou non donnée)."));
		return;
	}

	// Masquer l'indication du ciblage normal.
	if (IsValid(SelectedTarget))
	{
		SelectedTarget->SetTargetIndicationVisibility(false);
	}

	// Activer le ciblage de compétence.
	ActiveSkillTargeting = *Targeting;
	bSkillTargeting = true;
	TargetingMode = (ActiveSkillTargeting.Team == ESkillTargetTeam::Ally) ? ECombatTargetingMode::TargetAlly : ECombatTargetingMode::TargetEnemy;

	// Réinitialiser les cibles de compétence et l'aperçu.
	SkillPickedTargets.Reset();
	SkillPreviewTargets.Reset();

	// Une compétence a toujours une cible par défaut.
	if (AEonBound_Character* DefaultTarget = GetDefaultSkillTarget())
	{
		SkillPickedTargets.Add(DefaultTarget);
	}

	RefreshSkillPreview();

	UE_LOG(LogTemp, Warning, TEXT("[Combat] Ciblage de compétence : équipe %s, motif %d."),
		ActiveSkillTargeting.Team == ESkillTargetTeam::Ally ? TEXT("Ally") : TEXT("Enemy"),
		static_cast<int32>(ActiveSkillTargeting.Pattern));
}

void UEonBound_CombatManager::ConfirmSkill()
{
	AEonBound_Character* Caster = ActiveCombatant;

	if (!bSkillTargeting || CombatState != EEonBound_CombatState::PlayerTurn || !IsValid(Caster))
	{
		return;
	}

	TArray<AEonBound_Character*> Targets = ComputeSkillTargets();

	if (Targets.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Combat] Aucune cible choisie pour la compétence."));
		return;
	}

	Caster->SetCurrentActionTargets(Targets);
	SetCombatState(EEonBound_CombatState::ExecutingAction);

	const bool bActivated = (ActiveSlot == ECombatSkillSlot::Ultimate) ? Caster->TryUltimate() : Caster->TrySkill();

	if (!bActivated)
	{
		UE_LOG(LogTemp, Error, TEXT("[Combat] Skill activation failed. Returning to PlayerTurn."));
		SetCombatState(EEonBound_CombatState::PlayerTurn);
		Caster->SetCurrentActionTargets(TArray<AEonBound_Character*>());
		return;
	}

	EndSkillTargeting();

	if (CombatHUD)
	{
		CombatHUD->SetButtonVisibility(ECombatHUDState::Basic, false);
		CombatHUD->SetButtonVisibility(ECombatHUDState::Skill, false);
		CombatHUD->SetButtonVisibility(ECombatHUDState::Ult, false);
	}
}

void UEonBound_CombatManager::EndSkillTargeting()
{
	for (AEonBound_Character* Old : SkillPreviewTargets)
	{
		if (IsValid(Old))
		{
			Old->SetTargetIndicationVisibility(false);
		}
	}

	SkillPreviewTargets.Reset();
	SkillPickedTargets.Reset();
	bSkillTargeting = false;
	TargetingMode = ECombatTargetingMode::TargetEnemy;
}

void UEonBound_CombatManager::PickSkillTarget(AEonBound_Character* Clicked)
{
	if (!bSkillTargeting || !IsValid(Clicked) || !IsCombatantAlive(Clicked))
	{
		return;
	}

	// Vérifie que la cible cliquée appartient à l'équipe correcte.
	const TArray<TObjectPtr<AEonBound_Character>>& Team = ActiveSkillTargeting.Team == ESkillTargetTeam::Ally ? PlayerCombatants : EnemyCombatants;

	if (!Team.Contains(Clicked))
	{
		return;
	}

	switch (ActiveSkillTargeting.Pattern)
	{
	case ESkillTargetPattern::All:
		// Toute l'équipe est déjà visée : le clic ne change rien.
		return;

	case ESkillTargetPattern::MultiSelect:
		if (SkillPickedTargets.Contains(Clicked))
		{
			SkillPickedTargets.Remove(Clicked);
		}
		else if (SkillPickedTargets.Num() < ActiveSkillTargeting.MaxTargets)
		{
			SkillPickedTargets.Add(Clicked);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[Combat] Maximum de %d cibles atteint."), ActiveSkillTargeting.MaxTargets);
			return;
		}
		break;

	default:
		SkillPickedTargets.Reset();
		SkillPickedTargets.Add(Clicked);
		break;
	}

	RefreshSkillPreview();
}

void UEonBound_CombatManager::RefreshSkillPreview()
{
	for (AEonBound_Character* Old : SkillPreviewTargets)
	{
		if (IsValid(Old))
		{
			Old->SetTargetIndicationVisibility(false);
		}
	}

	SkillPreviewTargets.Reset();

	for (AEonBound_Character* Target : ComputeSkillTargets())
	{
		SkillPreviewTargets.Add(Target);
		Target->SetTargetIndicationVisibility(true);
	}
}

TArray<AEonBound_Character*> UEonBound_CombatManager::ComputeSkillTargets() const
{
	TArray<AEonBound_Character*> Result;

	// Détermine l'équipe à viser en fonction du ciblage de la compétence.
	const TArray<TObjectPtr<AEonBound_Character>>& Team = ActiveSkillTargeting.Team == ESkillTargetTeam::Ally ? PlayerCombatants : EnemyCombatants;

	switch (ActiveSkillTargeting.Pattern)
	{
		
	case ESkillTargetPattern::All:
		{
			for (AEonBound_Character* Member : Team)
			{
				if (IsCombatantAlive(Member))
				{
					Result.Add(Member);
				}
			}
			break;
		}

	case ESkillTargetPattern::MultiSelect:
		{
			for (AEonBound_Character* Picked : SkillPickedTargets)
			{
				if (IsCombatantAlive(Picked))
				{
					Result.Add(Picked);
				}
			}
			break;
		}

	case ESkillTargetPattern::Single:
	case ESkillTargetPattern::SingleAndAdjacent:
		{
			if (SkillPickedTargets.IsEmpty())
			{
				break;
			}

			AEonBound_Character* Center = SkillPickedTargets[0];

			if (!IsCombatantAlive(Center))
			{
				break;
			}

			AEonBound_Character* Previous = nullptr;
			AEonBound_Character* Next = nullptr;

			if (ActiveSkillTargeting.Pattern == ESkillTargetPattern::SingleAndAdjacent)
			{
				// Voisins = combattants vivants des emplacements les plus proches de chaque côté.
				for (AEonBound_Character* Member : Team)
				{
					if (Member == Center || !IsCombatantAlive(Member))
					{
						continue;
					}

					const int32 Slot = Member->GetSlotIndex();
					const int32 CenterSlot = Center->GetSlotIndex();

					if (Slot < CenterSlot && (!Previous || Slot > Previous->GetSlotIndex()))
					{
						Previous = Member;
					}
					else if (Slot > CenterSlot && (!Next || Slot < Next->GetSlotIndex()))
					{
						Next = Member;
					}
				}
			}

			if (Previous)
			{
				Result.Add(Previous);
			}
			Result.Add(Center);
			if (Next)
			{
				Result.Add(Next);
			}
			break;
		}
	}

	return Result;
}

AEonBound_Character* UEonBound_CombatManager::GetDefaultSkillTarget() const
{
	// Compétence offensive : l'ennemi déjà sélectionné, sinon le premier vivant.
	if (ActiveSkillTargeting.Team == ESkillTargetTeam::Enemy)
	{
		if (IsValid(SelectedTarget) && EnemyCombatants.Contains(SelectedTarget) && IsCombatantAlive(SelectedTarget))
		{
			return SelectedTarget;
		}

		for (AEonBound_Character* Enemy : EnemyCombatants)
		{
			if (IsCombatantAlive(Enemy))
			{
				return Enemy;
			}
		}

		return nullptr;
	}

	// Compétence de soutien : l'allié vivant qui a le moins de PV.
	AEonBound_Character* Lowest = nullptr;
	float LowestHealth = TNumericLimits<float>::Max();

	for (AEonBound_Character* Ally : PlayerCombatants)
	{
		if (!IsCombatantAlive(Ally))
		{
			continue;
		}

		const float Health = Ally->GetAbilitySystemComponent()->GetNumericAttribute(UEonBound_AttributeSet::GetHealthAttribute());

		if (Health < LowestHealth)
		{
			LowestHealth = Health;
			Lowest = Ally;
		}
	}

	return Lowest;
}

void UEonBound_CombatManager::StartGaugeCharging()
{
	UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}

	// Toutes les jauges démarrent à 0.
	for (AEonBound_Character* Combatant : PlayerCombatants)
	{
		if (IsValid(Combatant))
		{
			Combatant->SetActionGauge(0.f);
		}
	}

	for (AEonBound_Character* Combatant : EnemyCombatants)
	{
		if (IsValid(Combatant))
		{
			Combatant->SetActionGauge(0.f);
		}
	}

	LastGaugeTime = World->GetTimeSeconds();
	// Démarre le timer pour mettre à jour les jauges toutes les 0,05 secondes.
	World->GetTimerManager().SetTimer(GaugeTimerHandle, this, &UEonBound_CombatManager::TickGauges, 0.05f, true);
}

void UEonBound_CombatManager::TickGauges()
{
	UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}

	// Calcul du temps écoulé depuis la dernière mise à jour des jauges.
	const double Now = World->GetTimeSeconds();
	const float DeltaTime = static_cast<float>(Now - LastGaugeTime);
	LastGaugeTime = Now;

	// Si le combat est terminé, arrêter le timer.
	if (CombatState == EEonBound_CombatState::Victory || CombatState == EEonBound_CombatState::Defeat)
	{
		World->GetTimerManager().ClearTimer(GaugeTimerHandle);
		return;
	}

	// Fonction lambda pour charger les jauges d'une équipe.
	auto Charge = [this, DeltaTime](const TArray<TObjectPtr<AEonBound_Character>>& Team)
	{
		for (AEonBound_Character* Combatant : Team)
		{
			if (!IsCombatantAlive(Combatant))
			{
				continue;
			}

			Combatant->SetActionGauge(Combatant->GetActionGauge() + GetCombatantSpeed(Combatant) * GaugeFillRate * DeltaTime);
		}
	};

	Charge(PlayerCombatants);
	Charge(EnemyCombatants);
}