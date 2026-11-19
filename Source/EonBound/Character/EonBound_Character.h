// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "EonBound_Character.generated.h"

class UEonBound_AbilitySystemComponent;
class UEonBound_AttributeSet;	
class UGameplayAbility;
class UEonBound_GA_Skill;
struct FSkillTargeting;
class UEonBound_CombatManager;
class UFocusUserWidget;
class UWidgetComponent;
class ADamageNumberActor;
class UEonBound_GA_Ultimate;

UCLASS()
class EONBOUND_API AEonBound_Character : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = true))
	TObjectPtr<UEonBound_AbilitySystemComponent> EonBound_ASC;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = true))
	TObjectPtr<UEonBound_AttributeSet> EonBound_AttributeSet;
	
	UPROPERTY()
	TObjectPtr<UEonBound_CombatManager> CombatManager;
	
	void InitializeDefaultAttributes();

public:
	// Sets default values for this character's properties
	AEonBound_Character();
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	 
	UPROPERTY()
	bool bDefaultAttributesInitialized = false;
	
	UPROPERTY()
	bool bInitialAbilitiesGiven = false;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayAbility> BasicAttackAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UEonBound_GA_Skill> SkillAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UEonBound_GA_Ultimate> UltimateAbility;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UFocusUserWidget> FocusWidgetClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> FocusWidget;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsDead = false;
	
	UPROPERTY()
	TObjectPtr<AEonBound_Character> CurrentActionTarget;
	
	// Cibles des compétences (la basique continue d'utiliser CurrentActionTarget).
	UPROPERTY()
	TArray<TObjectPtr<AEonBound_Character>> CurrentActionTargets;

	// Emplacement dans la formation, copié depuis le point de spawn.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 SlotIndex = 0;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Damage Numbers")
	TSubclassOf<ADamageNumberActor> DamageNumberClass;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Damage Numbers")
	float DamageNumberHeight = 120.f;

public:
	void SetCombatManager(UEonBound_CombatManager* InCombatManager);
	
	UEonBound_CombatManager* GetCombatManager() const
	{
		return CombatManager.Get();
	}
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UFUNCTION(BlueprintCallable, Category = "GAS")
	void GiveInitialAbilities();
	
	void SetCurrentActionTarget(AEonBound_Character* NewTarget);
	
	AEonBound_Character* GetCurrentActionTarget() const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool TryBasicAttack();
		
	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool TrySkill();
	
	void SetCurrentActionTargets(const TArray<AEonBound_Character*>& NewTargets);

	const TArray<TObjectPtr<AEonBound_Character>>& GetCurrentActionTargets() const
	{
		return CurrentActionTargets;
	}
	
	// Réglage de l'emplacement dans la formation, copié depuis le point de spawn.
	void SetSlotIndex(int32 InSlotIndex) { SlotIndex = InSlotIndex; }
	int32 GetSlotIndex() const { return SlotIndex; }

	// Réglage de ciblage de la compétence, lu sur la classe (nullptr si le perso n'en a pas).
	const FSkillTargeting* GetSkillTargeting() const;
	// Vrai si la compétence est donnée, hors cooldown et utilisable.
	bool CanUseSkill() const;
	// Tours de cooldown restants sur la compétence (0 si disponible).
	int32 GetSkillCooldownRemaining() const;
	
	
	// Ultime : même logique que la compétence.
	bool TryUltimate();
	const FSkillTargeting* GetUltimateTargeting() const;
	// Vrai si l'ultime est donné, utilisable et que l'énergie suffit.
	bool CanUseUltimate() const;
	
	
	void AddEnergy(float Amount);
	float GetEnergy() const;
	float GetEnergyMax() const;
	
	void SetTargetIndicationVisibility(bool bVisible);
	
	void HandleDeath();
	
	bool IsDead() const { return bIsDead; }
	
	FDelegateHandle HealthChangedHandle;
	
	UFUNCTION(BlueprintCallable, Category = "Combat|Damage Numbers")
	void SpawnDamageNumber(float Amount, bool bIsCritical);
	
};
