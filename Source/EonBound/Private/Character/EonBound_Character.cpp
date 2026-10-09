// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/EonBound_Character.h"
#include "AbilitySystemComponent.h"
#include "GAS/EonBound_AbilitySystemComponent.h"
#include "Systems/EonBound_PlayerState.h"
#include "GAS/Effects/EonBound_GE_DefaultAttribute.h"

// Sets default values
AEonBound_Character::AEonBound_Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't 
 	// need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEonBound_Character::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEonBound_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEonBound_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UAbilitySystemComponent* AEonBound_Character::GetAbilitySystemComponent() const
{
	return EonBound_ASC;
}

void AEonBound_Character::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	TObjectPtr<AEonBound_PlayerState> EonBound_PS = GetPlayerState<AEonBound_PlayerState>();
	if (EonBound_PS)
	{
		EonBound_ASC = Cast<UEonBound_AbilitySystemComponent>(EonBound_PS->GetAbilitySystemComponent());
		if (EonBound_ASC)
		{
			EonBound_ASC->InitAbilityActorInfo(EonBound_PS, this);
		}
		InitializeDefaultAttributes();
	}
}

void AEonBound_Character::OnRep_PlayerState()
{
	TObjectPtr<AEonBound_PlayerState> EonBound_PS = GetPlayerState<AEonBound_PlayerState>();
	if (EonBound_PS)
	{
		EonBound_ASC = Cast<UEonBound_AbilitySystemComponent>(EonBound_PS->GetAbilitySystemComponent());
		if (EonBound_ASC)
		{
			EonBound_ASC->InitAbilityActorInfo(EonBound_PS, this);
		}
		InitializeDefaultAttributes();
	}
}

//TEMPORARY: Set the default value of the Chara using a Gameplay Effect
void AEonBound_Character::InitializeDefaultAttributes()
{
	if (!EonBound_ASC){return;}
	
	FGameplayEffectContextHandle EffectContext =
		EonBound_ASC->MakeEffectContext();

	FGameplayEffectSpecHandle EffectSpec = EonBound_ASC->MakeOutgoingSpec(
	UEonBound_GE_DefaultAttribute::StaticClass(),1.f,EffectContext);

	if (EffectSpec.IsValid())
	{
		EonBound_ASC->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());
	}
	
}