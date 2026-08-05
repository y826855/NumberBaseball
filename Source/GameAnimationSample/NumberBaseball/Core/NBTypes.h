#pragma once

#include "CoreMinimal.h"
#include "NBTypes.generated.h"

UENUM(BlueprintType)
enum class ENBGamePhase : uint8
{
	WaitingForReady,
	Intro,
	Playing,
	Outro,
	Finished
};

UENUM(BlueprintType)
enum class ENBTurnPhase : uint8
{
	StartingTurn,
	UserInputTurn,
	EndingTurn
};

UENUM(BlueprintType)
enum class ENBRoundPhase : uint8
{
	None,
	Starting,
	InProgress,
	Ended
};
