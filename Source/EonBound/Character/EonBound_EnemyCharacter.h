// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EonBound_Character.h"
#include "EonBound_EnemyCharacter.generated.h"

class UWidgetComponent;
class UEnemyStatut_UserWidget;
/**
 * 
 */
UCLASS()
class EONBOUND_API AEonBound_EnemyCharacter : public AEonBound_Character
{
	GENERATED_BODY()
	
public:
	AEonBound_EnemyCharacter();
	
	virtual void BeginPlay() override;
	
	bool bIsBoss = false;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> EnemyLifeBarWidgetComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UEnemyStatut_UserWidget> EnemyLifeBarWidgetClass;
	
};
