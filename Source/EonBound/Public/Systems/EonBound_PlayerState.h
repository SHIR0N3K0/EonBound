// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "EonBound_PlayerState.generated.h"

class UEonBound_AttributeSet;
class UEonBound_AbilitySystemComponent;
/**
 * 
 */
UCLASS()
class EONBOUND_API AEonBound_PlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UEonBound_AbilitySystemComponent> EonBound_ASC;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UEonBound_AttributeSet> EonBound_AttributeSet;
	
public:
	AEonBound_PlayerState();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
protected:	
	virtual void BeginPlay() override;
	
};
