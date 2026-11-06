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
	DamageWidget->SetWidgetClass(DamageWidgetClass);
	RootComponent = DamageWidget;
	DamageWidget->SetWidgetSpace(EWidgetSpace::Screen);
	DamageWidget->SetDrawSize(FVector2D(150.f, 50.f));
}

void ADamageNumberActor::InitializeDamage(float Amount, bool bIsCritical)
{
	UDamageNumber_UserWidget* DamageUserWidget = Cast<UDamageNumber_UserWidget>(DamageWidget->GetUserWidgetObject());
	
	if (!DamageUserWidget)
	{
		return;
	}
	
	DamageUserWidget->SetDamageValue(Amount, bIsCritical);
	
	GetWorldTimerManager().SetTimer(DamageNumberTimerHandle,this,&ADamageNumberActor::DestroyDamageNumber,DisplayDuration,false);
}

// Called when the game starts or when spawned
void ADamageNumberActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADamageNumberActor::DestroyDamageNumber()
{
	Destroy();
}

