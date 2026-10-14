// Fill out your copyright notice in the Description page of Project Settings.

#include "EonBound_PlayerControler.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EonBound_GameMode.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EonBound/Combat/EonBound_CombatManager.h"

AEonBound_PlayerControler::AEonBound_PlayerControler()
{
	
}

void AEonBound_PlayerControler::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("[Input] BeginPlay - Controller: %s | Local: %s | IMC: %s | IA: %s"),
		*GetNameSafe(this),
		IsLocalController() ? TEXT("true") : TEXT("false"),
		*GetNameSafe(InputMappingContext),
		*GetNameSafe(BasicAttackAction));

	if (IsLocalController())
	{
		if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (InputMappingContext)
				{
					InputSubsystem->AddMappingContext(InputMappingContext, 0);
					UE_LOG(LogTemp, Warning, TEXT("[Input] IMC added successfully: %s"),
						*GetNameSafe(InputMappingContext));
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("[Input] IMC is missing on %s."),
						*GetNameSafe(this));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[Input] Enhanced Input subsystem is missing."));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[Input] LocalPlayer is missing."));
		}
	}
}

void AEonBound_PlayerControler::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (BasicAttackAction)
		{
			EnhancedInputComponent->BindAction(BasicAttackAction,ETriggerEvent::Started,this,&AEonBound_PlayerControler::HandleBasicAttack);
			UE_LOG(LogTemp, Warning, TEXT("[Input] IA_BasicAttack bound: %s"),
				*GetNameSafe(BasicAttackAction));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[Input] IA_BasicAttack is missing on %s."),
				*GetNameSafe(this));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[Input] InputComponent is not an EnhancedInputComponent."));
	}
}

void AEonBound_PlayerControler::HandleBasicAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("[Input] IA_BasicAttack triggered."));

	if (!GetWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("[Input] HandleBasicAttack failed: World is missing."));
		return;
	}

	AEonBound_GameMode* GM = GetWorld()->GetAuthGameMode<AEonBound_GameMode>();

	if (!GM)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[Input] HandleBasicAttack blocked: AuthGameMode is missing. Controller authority: %s."),
			HasAuthority() ? TEXT("true") : TEXT("false"));
		return;
	}

	UEonBound_CombatManager* Manager = GM->GetCombatManager();

	if (!Manager)
	{
		UE_LOG(LogTemp, Error, TEXT("[Input] HandleBasicAttack blocked: CombatManager is missing."));
		return;
	}

	AActor* Target = Manager->PlayerTarget;
	if (!Target)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[Input] HandleBasicAttack blocked: no living enemy target was found."));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[Input] Requesting basic attack. Target: %s"),
		*GetNameSafe(Target));
	Manager->RequestPlayerBasicAttack(Target);
}
