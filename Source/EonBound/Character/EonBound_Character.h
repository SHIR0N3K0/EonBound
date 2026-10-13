// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Abilities/GameplayAbility.h"
#include "EonBound_Character.generated.h"

class UEonBound_AbilitySystemComponent;
class UEonBound_AttributeSet;
class UEonBound_CombatComponent;
class UGameplayAbility;

UCLASS()
class EONBOUND_API AEonBound_Character : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
	TObjectPtr<UEonBound_AbilitySystemComponent> EonBound_ASC;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UEonBound_AttributeSet> EonBound_AttributeSet;
	
	UPROPERTY()
	bool bDefaultAttributesInitialized = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<UGameplayAbility> BasicAttackAbility;
	
	void InitializeDefaultAttributes();

public:
	// Sets default values for this character's properties
	AEonBound_Character();
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UFUNCTION(BlueprintCallable, Category = "GAS")
	void GiveInitialAbilities();
	
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite,Category = "Combat")
	TObjectPtr<AActor> CurrentTarget;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void TryBasicAttack();
	
};
