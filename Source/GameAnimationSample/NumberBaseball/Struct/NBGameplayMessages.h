#pragma once

#include "CoreMinimal.h"
#include "GameAnimationSample/NumberBaseball/Core/NBTypes.h"
#include "NativeGameplayTags.h"
#include "NBGameplayMessages.generated.h"

namespace NBGameplayMessages
{
	GAMEANIMATIONSAMPLE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputSubmitted);
	GAMEANIMATIONSAMPLE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GamePhaseChanged);
	GAMEANIMATIONSAMPLE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TurnPhaseChanged);
	GAMEANIMATIONSAMPLE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputValuesChanged);
	GAMEANIMATIONSAMPLE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(PendingTaskChanged);
}

USTRUCT(BlueprintType)
struct GAMEANIMATIONSAMPLE_API FNBInputMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Number Baseball")
	int32 Value = 0;
};

USTRUCT(BlueprintType)
struct GAMEANIMATIONSAMPLE_API FNBGamePhaseChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Number Baseball")
	ENBGamePhase GamePhase = ENBGamePhase::WaitingForReady;
};

USTRUCT(BlueprintType)
struct GAMEANIMATIONSAMPLE_API FNBTurnPhaseChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Number Baseball")
	ENBTurnPhase TurnPhase = ENBTurnPhase::StartingTurn;
};

USTRUCT(BlueprintType)
struct GAMEANIMATIONSAMPLE_API FNBInputValuesChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Number Baseball")
	TArray<int32> InputValues;

	UPROPERTY(BlueprintReadOnly, Category = "Number Baseball")
	int32 RequiredInputCount = 0;
};
