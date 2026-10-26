// Fill out your copyright notice in the Description page of Project Settings.

#include "EonBound_PlayerControler.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraActor.h"
#include "EonBound/Character/EonBound_Character.h"
#include "Engine/EngineTypes.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EonBound/Combat/EonBound_CombatManager.h"

AEonBound_PlayerControler::AEonBound_PlayerControler()
{
	
}

void AEonBound_PlayerControler::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("[Input] BeginPlay - Controller: %s | Local: %s | IMC: %s"),*GetNameSafe(this),
		IsLocalController() ? TEXT("true") : TEXT("false"),*GetNameSafe(InputMappingContext));

	if (IsLocalController())
	{
		if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (InputMappingContext)
				{
					InputSubsystem->AddMappingContext(InputMappingContext, 0);
					UE_LOG(LogTemp, Warning, TEXT("[Input] IMC added successfully: %s"),*GetNameSafe(InputMappingContext));
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("[Input] IMC is missing on %s."),*GetNameSafe(this));
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
	
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	SetInputMode(InputMode);
	SetShowMouseCursor(true);
	
}

void AEonBound_PlayerControler::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (BasicAttackAction)
		{
			EnhancedInputComponent->BindAction(BasicAttackAction,ETriggerEvent::Started,this,&AEonBound_PlayerControler::HandleBasicAttack);
			UE_LOG(LogTemp, Warning, TEXT("[Input] IA_BasicAttack bound: %s"),*GetNameSafe(BasicAttackAction));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[Input] IA_BasicAttack is missing on %s."),*GetNameSafe(this));
		}
		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(InteractAction,ETriggerEvent::Started,this,&AEonBound_PlayerControler::HandleInteract);
			UE_LOG(LogTemp, Warning, TEXT("[Input] IA_Interact bound: %s"),*GetNameSafe(InteractAction));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[Input] IA_Interact is missing on %s."),*GetNameSafe(this));
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
	
	if (!Manager)
	{
		UE_LOG(LogTemp, Error, TEXT("[Input] HandleBasicAttack blocked: CombatManager is missing."));
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("[Input] Requesting basic attack. Target: %s"),*GetNameSafe(Manager->SelectedEnemyTarget));
	
	AEonBound_Character* EnemyTargeted = Manager->SelectedEnemyTarget;
	if (EnemyTargeted != nullptr)
	{
		Manager->RequestPlayerBasicAttack(EnemyTargeted);
	}
}

void AEonBound_PlayerControler::HandleInteract()
{
	FHitResult HitResultCursor;
	
	if (GetHitResultUnderCursor(ECC_GameTraceChannel1,true,HitResultCursor) && IsValid(Manager))
	{
		AActor* HitTarget = HitResultCursor.GetActor();

		if (AEonBound_Character* HitCharacter = Cast<AEonBound_Character>(HitTarget))
		{
			CursorTarget = HitCharacter;

			Manager->SetSelectedTarget(CursorTarget);

			UE_LOG(LogTemp, Warning, TEXT("[Input] Targeting character: %s"), *GetNameSafe(CursorTarget));
		}
	}
}

void AEonBound_PlayerControler::SetCameraActor(ACameraActor* NewCameraActor)
{
	if (!NewCameraActor)
	{
		UE_LOG(LogTemp, Error, TEXT("[Camera] SetCameraActor failed: NewCameraActor is null."));
		return;
	}
	
	CameraActor = NewCameraActor;
	
	if (CameraActor != nullptr)
	{
		SetViewTarget(CameraActor);
	}
}

ACameraActor* AEonBound_PlayerControler::GetCameraActor() const
{
	return CameraActor;
}
