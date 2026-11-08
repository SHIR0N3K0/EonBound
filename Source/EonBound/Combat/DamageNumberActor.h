// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageNumberActor.generated.h"

class UWidgetComponent;
class UDamageNumber_UserWidget;

UCLASS()
class EONBOUND_API ADamageNumberActor : public AActor
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage Number")
	TObjectPtr<UWidgetComponent> DamageWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Damage Number")
	float DisplayDuration = 0.8f;
	
	FTimerHandle DamageNumberTimerHandle;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage Number")
	TSubclassOf<UDamageNumber_UserWidget> DamageWidgetClass;
	
public:	
	// Sets default values for this actor's properties
	ADamageNumberActor();
	
	void InitializeDamage(float Amount, bool bIsCritical);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void DestroyDamageNumber();

};
