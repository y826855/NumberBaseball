#pragma once

#include "CoreMinimal.h"
#include "NBGuessResult.generated.h"

UENUM(BlueprintType)
enum class ENBNumberMatchResult : uint8
{
	None,
	Ball,
	Strike
};

USTRUCT(BlueprintType)
struct GAMEANIMATIONSAMPLE_API FNBNumberResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "GuessResult")
	int32 Number = 0;

	UPROPERTY(BlueprintReadOnly, Category = "GuessResult")
	ENBNumberMatchResult MatchResult = ENBNumberMatchResult::None;
};

USTRUCT(BlueprintType)
struct GAMEANIMATIONSAMPLE_API FNBGuessResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "GuessResult")
	int32 AttemptId = 0;

	UPROPERTY(BlueprintReadOnly, Category = "GuessResult")
	TArray<FNBNumberResult> NumberResults;

	UPROPERTY(BlueprintReadOnly, Category = "GuessResult")
	int32 StrikeCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "GuessResult")
	int32 BallCount = 0;
};
