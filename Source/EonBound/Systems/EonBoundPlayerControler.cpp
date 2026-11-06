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

	if (!IsLocalController())
	{
		return;
	}
	
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
		if (SkillAction)
		{
			EnhancedInputComponent->BindAction(SkillAction,ETriggerEvent::Started,this,&AEonBound_PlayerControler::HandleSkill);
			UE_LOG(LogTemp, Warning, TEXT("[Input] IA_Skill bound: %s"),*GetNameSafe(SkillAction));
		}
		if (CancelAction)
		{
			EnhancedInputComponent->BindAction(CancelAction,ETriggerEvent::Started,this,&AEonBound_PlayerControler::HandleCancel);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[Input] IA_Cancel is missing on %s."),*GetNameSafe(this));
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
	
	UE_LOG(LogTemp, Warning, TEXT("[Input] Requesting basic attack. Target: %s"),*GetNameSafe(Manager->SelectedTarget));
	
	AEonBound_Character* EnemyTargeted = Manager->SelectedTarget;
	
	if (!IsValid(EnemyTargeted))
	{
		return;
	}
	
	Manager->RequestPlayerBasicAttack(EnemyTargeted);
}

void AEonBound_PlayerControler::HandleSkill()
{
	UE_LOG(LogTemp, Warning,
		TEXT("[Input] IA_Skill triggered."));

	if (!IsValid(Manager))
	{
		return;
	}

	Manager->ToggleSkillTargeting();
}

void AEonBound_PlayerControler::HandleInteract()
{
	FHitResult HitResultCursor;
	
	if (!GetHitResultUnderCursor(ECC_GameTraceChannel1,true,HitResultCursor))
	{
		return;
	}

	if (!IsValid(Manager))
	{
		return;
	}

	AEonBound_Character* HitCharacter = Cast<AEonBound_Character>(HitResultCursor.GetActor());

	if (!IsValid(HitCharacter))
	{
		return;
	}

	UE_LOG(LogTemp, Warning,
		TEXT("[Input] Requesting target selection: %s"),
		*GetNameSafe(HitCharacter));

	Manager->SetSelectedTarget(HitCharacter);
}

void AEonBound_PlayerControler::HandleCancel()
{
	if (!IsValid(Manager))
	{
		return;
	}

	Manager->CancelTargeting();
}

void AEonBound_PlayerControler::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsLocalController())
	{
		if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (InputMappingContext)
				{
					InputSubsystem->RemoveMappingContext(InputMappingContext);
				}
			}
		}
	}
	
	Super::EndPlay(EndPlayReason);
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
		SetViewTarget(NewCameraActor);
	}
}

ACameraActor* AEonBound_PlayerControler::GetCameraActor() const
{
	return CameraActor;
}

void AEonBound_PlayerControler::SetManagerREF(UEonBound_CombatManager* InManager)
{
	if (!InManager)
	{
		UE_LOG(LogTemp, Error, TEXT("[Combat] SetManagerREF failed: InManager is null."));
		return;
	}
	
	Manager = InManager;
}
