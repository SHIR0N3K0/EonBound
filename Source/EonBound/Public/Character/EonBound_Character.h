// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "EonBound_Character.generated.h"

class UEonBound_AbilitySystemComponent;
class UAEonBound_CombatComponent;

UCLASS()
class EONBOUND_API AEonBound_Character : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UEonBound_AbilitySystemComponent> EonBound_ASC;

public:
	// Sets default values for this character's properties
	AEonBound_Character();
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void PossessedBy(AController* NewController) override;
	
	int Health;
	int MaxHealth;
	int Attack;
	int Defense;
	int Speed;
};
