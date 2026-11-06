// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "EonBound_Character.generated.h"

class UEonBound_AbilitySystemComponent;
class UEonBound_AttributeSet;	
class UGameplayAbility;
class UEonBound_CombatManager;
class UFocusUserWidget;
class UWidgetComponent;
class ADamageNumberActor;

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
	TSubclassOf<UGameplayAbility> SkillAbility;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UFocusUserWidget> FocusWidgetClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> FocusWidget;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsDead = false;
	
	UPROPERTY()
	TObjectPtr<AEonBound_Character> CurrentActionTarget;
	
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
	
	void SetTargetIndicationVisibility(bool bVisible);
	
	void HandleDeath();
	
	bool IsDead() const { return bIsDead; }
	
	FDelegateHandle HealthChangedHandle;
	
	UFUNCTION(BlueprintCallable, Category = "Combat|Damage Numbers")
	void SpawnDamageNumber(float Amount, bool bIsCritical);
	
};
