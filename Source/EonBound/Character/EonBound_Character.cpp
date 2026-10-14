// Fill out your copyright notice in the Description page of Project Settings.


#include "EonBound_Character.h"
#include "AbilitySystemComponent.h"
#include "EonBound/Combat/EonBound_CombatManager.h"
#include "EonBound/GAS/EonBound_AbilitySystemComponent.h"
#include "EonBound/GAS/EonBound_AttributeSet.h"
#include "EonBound/GAS/Abilities/EonBound_GA_NormalAttack.h"
#include "EonBound/GAS/Effects/EonBound_GE_DefaultAttribute.h"

// Sets default values
AEonBound_Character::AEonBound_Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't 
 	// need it.
	PrimaryActorTick.bCanEverTick = true;
	
	EonBound_ASC = CreateDefaultSubobject<UEonBound_AbilitySystemComponent>("EonBound_ASC");
	EonBound_ASC->SetIsReplicated(true);
	EonBound_ASC->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	SetNetUpdateFrequency(70);
	EonBound_AttributeSet = CreateDefaultSubobject<UEonBound_AttributeSet>("EonBound_AttributeSet");
	
	BasicAttackAbility = UEonBound_GA_NormalAttack::StaticClass();
}

// Called when the game starts or when spawned
void AEonBound_Character::BeginPlay()
{
	Super::BeginPlay();
	
	if (EonBound_ASC)
	{
		EonBound_ASC->InitAbilityActorInfo(this, this);
		InitializeDefaultAttributes();
	}
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

void AEonBound_Character::SetCombatManager(UEonBound_CombatManager* InCombatManager)
{
	CombatManager = InCombatManager;
}

void AEonBound_Character::GiveInitialAbilities()
{
	if (!HasAuthority() || !EonBound_ASC)
	{
		return;
	}
	
	if (BasicAttackAbility)
	{
		EonBound_ASC->GiveAbility(FGameplayAbilitySpec(BasicAttackAbility, 1, 0));
	}
}

void AEonBound_Character::TryBasicAttack()
{
	if (!CurrentTarget || !EonBound_ASC)
	{
		return;
	}
	
	FGameplayTag AttackTag =FGameplayTag::RequestGameplayTag(FName("Ability.BasicAttack"));

	const bool bActivated =
	EonBound_ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(AttackTag));

	if (bActivated)
	{
		UE_LOG(LogTemp, Log, TEXT("Basic attack ability activated successfully."));
	}
	else
	{
		UE_LOG(LogTemp, Warning,
			TEXT("Basic attack ability activation failed. Check that the ability is granted, has the tag Ability.BasicAttack, and is not blocked."));
	}
}

//TEMPORARY: Set the default value of the Chara using a Gameplay Effect
void AEonBound_Character::InitializeDefaultAttributes()
{
	if (!EonBound_ASC || bDefaultAttributesInitialized)
	{
		return;
	}
	
	FGameplayEffectContextHandle EffectContext = EonBound_ASC->MakeEffectContext();

	FGameplayEffectSpecHandle EffectSpec = EonBound_ASC->MakeOutgoingSpec(
	UEonBound_GE_DefaultAttribute::StaticClass(),1.f,EffectContext);

	if (EffectSpec.IsValid())
	{
		EonBound_ASC->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());
		bDefaultAttributesInitialized = true;
	}
	
	GiveInitialAbilities();
}