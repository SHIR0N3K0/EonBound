// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EonBound_CombatSpawnPoint.generated.h"

UENUM(BlueprintType)
enum class ECombatSpawnTeam : uint8
{
	Ally,
	Enemy
};

UCLASS()
class EONBOUND_API AEonBound_CombatSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEonBound_CombatSpawnPoint();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat Spawn")
	ECombatSpawnTeam Team = ECombatSpawnTeam::Ally;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat Spawn")
	int32 SpawnOrder = 0;

};
