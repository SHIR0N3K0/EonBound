// Fill out your copyright notice in the Description page of Project Settings.


#include "EonBound/Combat/EonBound_CombatSpawnPoint.h"

#include "Components/SceneComponent.h"
#include "Components/ArrowComponent.h"

// Sets default values
AEonBound_CombatSpawnPoint::AEonBound_CombatSpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneRoot);

	UArrowComponent* Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("SpawnDirection"));
	Arrow->SetupAttachment(SceneRoot);
}
