// Fill out your copyright notice in the Description page of Project Settings.

#include "EonBound_EnemyCharacter.h"

#include "Components/WidgetComponent.h"
#include "EonBound/GAS/EonBound_AbilitySystemComponent.h"
#include "EonBound/GAS/EonBound_AttributeSet.h"
#include "EonBound/UI/EnemyStatut_UserWidget.h"


AEonBound_EnemyCharacter::AEonBound_EnemyCharacter()
{
	EnemyLifeBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("EnemyLifeBarWidget"));
	EnemyLifeBarWidgetComponent->SetupAttachment(RootComponent);
}

void AEonBound_EnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (UEonBound_AbilitySystemComponent* ASC = EonBound_ASC)
	{
		ASC->SetNumericAttributeBase(UEonBound_AttributeSet::GetSpeedAttribute(),50.0f);
		ASC->SetNumericAttributeBase(UEonBound_AttributeSet::GetAttackAttribute(),40.0f);
	}
	
	if (EnemyLifeBarWidgetClass && IsValid(EonBound_ASC))
	{
		EnemyLifeBarWidgetComponent->SetWidgetClass(EnemyLifeBarWidgetClass);
		EnemyLifeBarWidgetComponent->InitWidget();
		
		if (UEnemyStatut_UserWidget* EnemyWidget =Cast<UEnemyStatut_UserWidget>(EnemyLifeBarWidgetComponent->GetUserWidgetObject()))
		{
			EnemyWidget->InitializeWidgetInfo(this, EonBound_ASC);
		}
		else
		{
			UE_LOG(LogTemp, Warning,TEXT("[%s] Enemy life bar widget instance is invalid."),*GetName());
		}
	}
}
