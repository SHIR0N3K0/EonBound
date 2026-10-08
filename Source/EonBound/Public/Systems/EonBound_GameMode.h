// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EonBound_GameMode.generated.h"

class AEonBound_PlayerControler;
/**
 * 
 */
UCLASS()
class EONBOUND_API AEonBound_GameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	UPROPERTY()
	AEonBound_PlayerControler* EonBound_PC;
	
	UFUNCTION()
	void SetController();
};
