#pragma once

#include "CoreMinimal.h"
#include "EonBound_SkillTypes.generated.h"

// Qui la compétence peut viser.
UENUM(BlueprintType)
enum class ESkillTargetTeam : uint8
{
	Enemy,
	Ally
};

// Combien de cibles et comment elles sont choisies.
UENUM(BlueprintType)
enum class ESkillTargetPattern : uint8
{
	Single,
	SingleAndAdjacent,
	MultiSelect,
	All
};

USTRUCT(BlueprintType)
struct FSkillTargeting
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ESkillTargetTeam Team = ESkillTargetTeam::Enemy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ESkillTargetPattern Pattern = ESkillTargetPattern::Single;

	// Utilisé seulement par MultiSelect.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1", EditCondition = "Pattern == ESkillTargetPattern::MultiSelect"))int32 MaxTargets = 2;
};