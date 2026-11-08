// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageNumberActor.h"

#include "Components/WidgetComponent.h"
#include "EonBound/UI/DamageNumber_UserWidget.h"
#include "TimerManager.h"

// Sets default values
ADamageNumberActor::ADamageNumberActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	DamageWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamageWidget"));
	RootComponent = DamageWidget;
	DamageWidget->SetWidgetSpace(EWidgetSpace::Screen);
	DamageWidget->SetDrawSize(FVector2D(150.f, 50.f));
}

void ADamageNumberActor::InitializeDamage(float Amount, bool bIsCritical)
{
	UUserWidget* Widget = DamageWidget->GetUserWidgetObject();

	if (!Widget)
	{
		DamageWidget->InitWidget();
		Widget = DamageWidget->GetUserWidgetObject();
	}

	UDamageNumber_UserWidget* DamageUserWidget = Cast<UDamageNumber_UserWidget>(Widget);

	if (!DamageUserWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[DamageNumber] Widget is null or not a UDamageNumber_UserWidget (class: %s)."),
			*GetNameSafe(Widget ? Widget->GetClass() : nullptr));
		Destroy();
		return;
	}

	DamageUserWidget->SetDamageValue(Amount, bIsCritical);

	GetWorldTimerManager().SetTimer(DamageNumberTimerHandle, this, &ADamageNumberActor::DestroyDamageNumber, DisplayDuration, false);
}

// Called when the game starts or when spawned
void ADamageNumberActor::BeginPlay()
{
	Super::BeginPlay();

	if (DamageWidgetClass)
	{
		DamageWidget->SetWidgetClass(DamageWidgetClass);
		DamageWidget->InitWidget();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[DamageNumber] DamageWidgetClass is not assigned in %s."), *GetName());
	}
}

void ADamageNumberActor::DestroyDamageNumber()
{
	Destroy();
}

