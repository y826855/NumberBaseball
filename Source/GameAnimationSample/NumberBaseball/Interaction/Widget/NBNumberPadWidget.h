#pragma once

#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameAnimationSample/NumberBaseball/Struct/NBGuessResult.h"
#include "NBNumberPadWidget.generated.h"

struct FGameplayTag;
struct FNBInputValuesChangedMessage;
struct FNBPendingTaskState;
struct FNBRoundStateChangedMessage;
struct FNBTurnPhaseChangedMessage;

UCLASS(Abstract)
class GAMEANIMATIONSAMPLE_API UNBNumberPadWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "NumberPad")
	void FinishResultPresentation();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "NumberPad")
	void OnInputValueAdded(int32 InputValue);

	UFUNCTION(BlueprintImplementableEvent, Category = "NumberPad")
	void OnTurnStarted();

	UFUNCTION(BlueprintImplementableEvent, Category = "NumberPad")
	void OnRoundStarted(
		int32 CurrentRound,
		int32 TotalRoundCount,
		int32 RequiredInputCount);

	UFUNCTION(BlueprintImplementableEvent, Category = "NumberPad")
	void OnRoundEnded(int32 CompletedRound);

	UFUNCTION(BlueprintImplementableEvent, Category = "NumberPad")
	void OnGuessResultsUpdated(const TArray<ENBNumberMatchResult>& GuessResults);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 DisplayedInputCount = 0;
	
private:
	void ApplyInputValues(const TArray<int32>& InputValues);
	void ApplyGuessResult(const FNBGuessResult& GuessResult);

	void HandleInputValuesChanged(
		FGameplayTag Channel,
		const FNBInputValuesChangedMessage& Message);
	void HandleTurnPhaseChanged(
		FGameplayTag Channel,
		const FNBTurnPhaseChangedMessage& Message);
	void HandleRoundStateChanged(
		FGameplayTag Channel,
		const FNBRoundStateChangedMessage& Message);
	void HandleGuessResultChanged(
		FGameplayTag Channel,
		const FNBGuessResult& GuessResult);
	void HandlePendingTaskChanged(
		FGameplayTag Channel,
		const FNBPendingTaskState& PendingTaskState);

	FGameplayMessageListenerHandle InputValuesChangedListenerHandle;
	FGameplayMessageListenerHandle TurnPhaseChangedListenerHandle;
	FGameplayMessageListenerHandle RoundStateChangedListenerHandle;
	FGameplayMessageListenerHandle GuessResultChangedListenerHandle;
	FGameplayMessageListenerHandle PendingTaskChangedListenerHandle;
	int32 ActivePendingTaskId = INDEX_NONE;
};
