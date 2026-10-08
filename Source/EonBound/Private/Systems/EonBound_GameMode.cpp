// Fill out your copyright notice in the Description page of Project Settings.


#include "Systems/EonBound_GameMode.h"

void AEonBound_GameMode::SetController()
{
	APlayerController* CurrentPlayerController = GetWorld()->GetFirstPlayerController();
	
	if (CurrentPlayerController != nullptr)
	{
		return;
	}
}
