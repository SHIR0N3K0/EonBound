// Fill out your copyright notice in the Description page of Project Settings.


#include "EonBound_Character.h"

#include "AbilitySystemComponent.h"
#include "EonBound/UI/FocusUserWidget.h"
#include "Components/WidgetComponent.h"
#include "EonBound/Combat/EonBound_CombatManager.h"
#include "EonBound/GAS/EonBound_AbilitySystemComponent.h"
#include "EonBound/GAS/EonBound_AttributeSet.h"
#include "EonBound/GAS/Effects/EonBound_GE_DefaultAttribute.h"
#include "EonBound/Combat/DamageNumberActor.h"

// Sets default values
AEonBound_Character::AEonBound_Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't 
 	// need it.
	PrimaryActorTick.bCanEverTick = false;
	
	EonBound_ASC = CreateDefaultSubobject<UEonBound_AbilitySystemComponent>("EonBound_ASC");
	EonBound_ASC->SetIsReplicated(true);
	EonBound_ASC->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	SetNetUpdateFrequency(70);
	EonBound_AttributeSet = CreateDefaultSubobject<UEonBound_AttributeSet>("EonBound_AttributeSet");
	
	FocusWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("FocusWidget"));
	FocusWidget->SetupAttachment(RootComponent);

	
}

// Called when the game starts or when spawned
void AEonBound_Character::BeginPlay()
{
	Super::BeginPlay();
	
	if (EonBound_ASC)
	{
		EonBound_ASC->InitAbilityActorInfo(this, this);
		
		if (HasAuthority() && !bDefaultAttributesInitialized)
		{
			InitializeDefaultAttributes();
			GiveInitialAbilities();
		}
				
		HealthChangedHandle = EonBound_ASC->GetGameplayAttributeValueChangeDelegate(UEonBound_AttributeSet::GetHealthAttribute())
			.AddWeakLambda(this,[this](const FOnAttributeChangeData& Data)
			{
				if (Data.NewValue <= 0.f)
				{
					HandleDeath();
				}
			});
		
	}
	
	if (FocusWidgetClass && IsValid(FocusWidget))
	{
		FocusWidget->SetWidgetClass(FocusWidgetClass);
	}
}

UAbilitySystemComponent* AEonBound_Character::GetAbilitySystemComponent() const
{
	return EonBound_ASC;
}

void AEonBound_Character::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	
	if (EonBound_ASC && HealthChangedHandle.IsValid())
	{
		EonBound_ASC->GetGameplayAttributeValueChangeDelegate(UEonBound_AttributeSet::GetHealthAttribute()).Remove(HealthChangedHandle);
		HealthChangedHandle.Reset();
	}

	Super::EndPlay(EndPlayReason);
}

void AEonBound_Character::SetCombatManager(UEonBound_CombatManager* InCombatManager)
{
	CombatManager = InCombatManager;
}

void AEonBound_Character::GiveInitialAbilities()
{
	if (!HasAuthority() || !EonBound_ASC || bInitialAbilitiesGiven)
	{
		return;
	}
	
	if (BasicAttackAbility)
	{
		EonBound_ASC->GiveAbility(FGameplayAbilitySpec(BasicAttackAbility, 1, 0));
	}
	
	if (SkillAbility)
	{
		EonBound_ASC->GiveAbility(FGameplayAbilitySpec(SkillAbility, 1, 0));
	}
	
	bInitialAbilitiesGiven = true;
}

void AEonBound_Character::SetCurrentActionTarget(AEonBound_Character* NewTarget)
{
	CurrentActionTarget = NewTarget;
}

AEonBound_Character* AEonBound_Character::GetCurrentActionTarget() const
{
	return CurrentActionTarget.Get();
}

bool AEonBound_Character::TryBasicAttack()
{
	if (!CurrentActionTarget || !EonBound_ASC)
	{
		return false;
	}
	
	if (bIsDead)
	{
		return false;
	}
	FGameplayTag AttackTag =FGameplayTag::RequestGameplayTag(FName("Ability.BasicAttack"));
	
	const bool bActivated = EonBound_ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(AttackTag));

	if (bActivated)
	{
		UE_LOG(LogTemp, Log, TEXT("Basic attack ability activated successfully."));
	}
	else
	{
		UE_LOG(LogTemp, Warning,TEXT("Basic attack ability activation failed. Check that the ability is granted, has the tag Ability.BasicAttack, and is not blocked."));
	}
	return bActivated;
}

bool AEonBound_Character::TrySkill()
{
	if (!CurrentActionTarget || !EonBound_ASC)
	{
		return false;
	}
	
	if (bIsDead)
	{
		return false;
	}
	
	FGameplayTag SkillTag =FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Heal"));
	
	const bool bActivated = EonBound_ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(SkillTag));

	if (bActivated)
	{
		UE_LOG(LogTemp, Log, TEXT("Skill ability activated successfully."));
	}
	else
	{
		UE_LOG(LogTemp, Warning,TEXT("Skill ability activation failed. Check that the ability is granted, has the tag Ability.Skill, and is not blocked."));
	}
	return bActivated;
}

void AEonBound_Character::SetTargetIndicationVisibility(bool bVisible)
{
	if (FocusWidget)
	{
		FocusWidget->SetVisibility(bVisible);
	}
}

void AEonBound_Character::HandleDeath()
{
	if (bIsDead)
	{
		return;
	}
	
	bIsDead = true;
	
	// Masquer le personnage
	SetActorHiddenInGame(true);

	// Empêcher les collisions et les interactions physiques
	SetActorEnableCollision(false);

	// Arrêter son Tick
	SetActorTickEnabled(false);

	UE_LOG(LogTemp, Warning,TEXT("[Combat] %s is dead."), *GetName());
}

void AEonBound_Character::SpawnDamageNumber(float Amount, bool bIsCritical)
{
	if (!DamageNumberClass || !GetWorld())
	{
		return;
	}

	// Position au-dessus du personnage
	FVector SpawnLocation = GetActorLocation();
	SpawnLocation.Z += DamageNumberHeight;

	// Paramètres de spawn
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	// Spawn du Damage Number
	ADamageNumberActor* DamageActor = GetWorld()->SpawnActor<ADamageNumberActor>(DamageNumberClass,SpawnLocation,FRotator::ZeroRotator,SpawnParams);
	
	if (!DamageActor)
	{
		return;
	}

	// Initialisation du Widget
	DamageActor->InitializeDamage(Amount, bIsCritical);
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
}