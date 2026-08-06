#pragma once

#include "CoreMinimal.h"
#include "GameAnimationSample/NumberBaseball/Core/NBTypes.h"
#include "NativeGameplayTags.h"
#include "NBGameplayMessages.generated.h"

class APlayerState;

namespace NBGameplayMessages
{
	GAMEANIMATIONSAMPLE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputSubmitted);
	GAMEANIMATIONSAMPLE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GamePhaseChanged);
	GAMEANIMATIONSAMPLE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TurnPhaseChanged);
	GAMEANIMATIONSAMPLE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputValuesChanged);
	GAMEANIMATIONSAMPLE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(RoundStateChanged);
	GAMEANIMATIONSAMPLE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GuessResultChanged);
	GAMEANIMATIONSAMPLE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(PendingTaskChanged);
	GAMEANIMATIONSAMPLE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TimerChanged);
	GAMEANIMATIONSAMPLE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(NotifyReceived);
	GAMEANIMATIONSAMPLE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TurnPlayerChanged);
	GAMEANIMATIONSAMPLE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ScoreChanged);
}

namespace NBTimerTypes
{
	GAMEANIMATIONSAMPLE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameStart);
	GAMEANIMATIONSAMPLE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UserInput);
}

namespace NBNotifyTypes
{
	GAMEANIMATIONSAMPLE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(WaitingForPlayers);
	GAMEANIMATIONSAMPLE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(WaitingForGameStart);
	GAMEANIMATIONSAMPLE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(YourTurn);
	GAMEANIMATIONSAMPLE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(OpponentTurn);
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
struct GAMEANIMATIONSAMPLE_API FNBTurnPlayerChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Turn")
	TObjectPtr<APlayerState> TurnPlayer;
};

USTRUCT(BlueprintType)
struct GAMEANIMATIONSAMPLE_API FNBScoreChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Score")
	TObjectPtr<APlayerState> PlayerState;

	UPROPERTY(BlueprintReadOnly, Category = "Score")
	int32 CorrectAnswerCount = 0;
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

USTRUCT(BlueprintType)
struct GAMEANIMATIONSAMPLE_API FNBRoundStateChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Number Baseball")
	ENBRoundPhase RoundPhase = ENBRoundPhase::None;

	UPROPERTY(BlueprintReadOnly, Category = "Number Baseball")
	int32 CurrentRound = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Number Baseball")
	int32 TotalRoundCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Number Baseball")
	int32 RequiredInputCount = 0;
};
