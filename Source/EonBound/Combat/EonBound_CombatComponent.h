// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EonBound_CombatantData.h"
#include "GameFramework/Actor.h"
#include "EonBound_CombatComponent.generated.h"

UCLASS(ClassGroup=(Combat), meta=(BlueprintSpawnableComponent))
class EONBOUND_API UEonBound_CombatComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UEonBound_CombatComponent();
	virtual void BeginPlay() override;
	
private:

	UPROPERTY()
	bool bIsInCombat = false;

	UPROPERTY()
	bool bIsTakingTurn = false;
	
	UPROPERTY()
	FCombatantState CombatantState;

public:

	void EnterCombat();
	void LeaveCombat();

	void StartTurn();
	void EndTurn();

	bool IsInCombat() const;
	bool IsTakingTurn() const;

};
