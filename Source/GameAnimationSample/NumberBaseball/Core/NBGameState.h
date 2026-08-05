#pragma once

#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/GameState.h"
#include "GameAnimationSample/NumberBaseball/Struct/NBGuessResult.h"
#include "GameAnimationSample/NumberBaseball/Struct/NBPendingTask.h"
#include "NBTypes.h"
#include "NBGameState.generated.h"

struct FGameplayTag;
struct FNBInputMessage;

UCLASS()
class GAMEANIMATIONSAMPLE_API ANBGameState : public AGameState
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Number Baseball|Game State")
	void SetCurrentGamePhase(ENBGamePhase NewGamePhase);

	UFUNCTION(BlueprintCallable, Category = "Number Baseball|Game State")
	void SetCurrentTurnPhase(ENBTurnPhase NewTurnPhase);

	void SetCurrentRoundPhase(ENBRoundPhase NewRoundPhase);

	UFUNCTION(BlueprintCallable, Category = "Number Baseball|Game State")
	void AddInputValue(int32 Value);

	UFUNCTION(BlueprintCallable, Category = "Number Baseball|Game State")
	void ClearInputValues();

	void SetRequiredInputCount(int32 Count);
	void SetRoundState(int32 NewCurrentRound, int32 NewTotalRoundCount);
	void SetUserInputEndServerTime(float EndServerTime);
	void SetLastGuessResult(const FNBGuessResult& NewGuessResult);
	void SetPendingTaskState(const FNBPendingTaskState& NewPendingTaskState);

	UFUNCTION(BlueprintPure, Category = "Number Baseball|Game State")
	ENBGamePhase GetCurrentGamePhase() const { return CurrentGamePhase; }

	UFUNCTION(BlueprintPure, Category = "Number Baseball|Game State")
	ENBTurnPhase GetCurrentTurnPhase() const { return CurrentTurnPhase; }

	UFUNCTION(BlueprintPure, Category = "Number Baseball|Game State")
	ENBRoundPhase GetCurrentRoundPhase() const { return CurrentRoundPhase; }

	UFUNCTION(BlueprintPure, Category = "Number Baseball|Game State")
	TArray<int32> GetCurrentInputValues() const { return CurrentInputValues; }

	UFUNCTION(BlueprintPure, Category = "Number Baseball|Game State")
	int32 GetRequiredInputCount() const { return RequiredInputCount; }

	UFUNCTION(BlueprintPure, Category = "Number Baseball|Game State")
	int32 GetCurrentRound() const { return CurrentRound; }

	UFUNCTION(BlueprintPure, Category = "Number Baseball|Game State")
	int32 GetTotalRoundCount() const { return TotalRoundCount; }

	UFUNCTION(BlueprintPure, Category = "Number Baseball|Game State")
	float GetRemainingUserInputTime() const;

	UFUNCTION(BlueprintPure, Category = "Number Baseball|Game State")
	bool IsLastAnswerCorrect() const
	{
		return RequiredInputCount > 0
			&& LastGuessResult.NumberResults.Num() == RequiredInputCount
			&& LastGuessResult.StrikeCount == RequiredInputCount;
	}

	UFUNCTION(BlueprintPure, Category = "GuessResult")
	FNBGuessResult GetLastGuessResult() const { return LastGuessResult; }

	UFUNCTION(BlueprintPure, Category = "PendingTask")
	FNBPendingTaskState GetPendingTaskState() const { return PendingTaskState; }

private:
	void HandleInputSubmitted(FGameplayTag Channel, const FNBInputMessage& Message);

	UFUNCTION()
	void OnRep_CurrentGamePhase();

	UFUNCTION()
	void OnRep_CurrentTurnPhase();

	UFUNCTION()
	void OnRep_CurrentInputValues();

	UFUNCTION()
	void OnRep_CurrentRoundPhase();

	UFUNCTION()
	void OnRep_LastGuessResult();

	UFUNCTION()
	void OnRep_PendingTaskState();

	UPROPERTY(ReplicatedUsing = OnRep_CurrentGamePhase)
	ENBGamePhase CurrentGamePhase = ENBGamePhase::WaitingForReady;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentTurnPhase)
	ENBTurnPhase CurrentTurnPhase = ENBTurnPhase::StartingTurn;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentInputValues)
	TArray<int32> CurrentInputValues;

	UPROPERTY(Replicated)
	ENBRoundPhase CurrentRoundPhase = ENBRoundPhase::None;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentRoundPhase)
	int32 RoundStateRevision = 0;

	UPROPERTY(Replicated)
	int32 RequiredInputCount = 0;

	UPROPERTY(Replicated)
	int32 CurrentRound = 0;

	UPROPERTY(Replicated)
	int32 TotalRoundCount = 0;

	UPROPERTY(Replicated)
	float UserInputEndServerTime = 0.f;

	UPROPERTY(ReplicatedUsing = OnRep_LastGuessResult)
	FNBGuessResult LastGuessResult;

	UPROPERTY(ReplicatedUsing = OnRep_PendingTaskState)
	FNBPendingTaskState PendingTaskState;

	FGameplayMessageListenerHandle InputSubmittedListenerHandle;
};
