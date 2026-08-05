#include "NBNumberPadWidget.h"

#include "Engine/World.h"
#include "GameAnimationSample/NumberBaseball/Core/NBGameState.h"
#include "GameAnimationSample/NumberBaseball/PlayerController/Components/NBPendingTaskClientComponent.h"
#include "GameAnimationSample/NumberBaseball/PlayerController/NBPlayerController.h"
#include "GameAnimationSample/NumberBaseball/Struct/NBGameplayMessages.h"
#include "GameAnimationSample/NumberBaseball/Struct/NBPendingTask.h"

void UNBNumberPadWidget::FinishResultPresentation()
{
	if (ActivePendingTaskId == INDEX_NONE)
	{
		return;
	}

	ANBPlayerController* NBPlayerController = Cast<ANBPlayerController>(GetOwningPlayer());
	if (IsValid(NBPlayerController) == false)
	{
		return;
	}

	UNBPendingTaskClientComponent* PendingTaskClientComponent =
		NBPlayerController->GetPendingTaskClientComponent();
	if (IsValid(PendingTaskClientComponent) == false)
	{
		return;
	}

	const int32 CompletedTaskId = ActivePendingTaskId;
	ActivePendingTaskId = INDEX_NONE;
	PendingTaskClientComponent->NotifyPendingTaskFinished(CompletedTaskId);
}

void UNBNumberPadWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InputValuesChangedListenerHandle = UGameplayMessageSubsystem::Get(this)
		.RegisterListener<FNBInputValuesChangedMessage>(
			NBGameplayMessages::InputValuesChanged,
			this,
			&ThisClass::HandleInputValuesChanged);
	TurnPhaseChangedListenerHandle = UGameplayMessageSubsystem::Get(this)
		.RegisterListener<FNBTurnPhaseChangedMessage>(
			NBGameplayMessages::TurnPhaseChanged,
			this,
			&ThisClass::HandleTurnPhaseChanged);
	RoundStateChangedListenerHandle = UGameplayMessageSubsystem::Get(this)
		.RegisterListener<FNBRoundStateChangedMessage>(
			NBGameplayMessages::RoundStateChanged,
			this,
			&ThisClass::HandleRoundStateChanged);
	GuessResultChangedListenerHandle = UGameplayMessageSubsystem::Get(this)
		.RegisterListener<FNBGuessResult>(
			NBGameplayMessages::GuessResultChanged,
			this,
			&ThisClass::HandleGuessResultChanged);
	PendingTaskChangedListenerHandle = UGameplayMessageSubsystem::Get(this)
		.RegisterListener<FNBPendingTaskState>(
			NBGameplayMessages::PendingTaskChanged,
			this,
			&ThisClass::HandlePendingTaskChanged);

	if (ANBGameState* NBGameState = GetWorld()->GetGameState<ANBGameState>())
	{
		ApplyInputValues(NBGameState->GetCurrentInputValues());
		ApplyGuessResult(NBGameState->GetLastGuessResult());
		HandlePendingTaskChanged(FGameplayTag(), NBGameState->GetPendingTaskState());

		if (NBGameState->GetCurrentRoundPhase() == ENBRoundPhase::Starting
			|| NBGameState->GetCurrentRoundPhase() == ENBRoundPhase::InProgress)
		{
			OnRoundStarted(
				NBGameState->GetCurrentRound(),
				NBGameState->GetTotalRoundCount(),
				NBGameState->GetRequiredInputCount());
		}
		else if (NBGameState->GetCurrentRoundPhase() == ENBRoundPhase::Ended)
		{
			OnRoundEnded(NBGameState->GetCurrentRound());
		}

		if (NBGameState->GetCurrentRoundPhase() == ENBRoundPhase::InProgress
			&& NBGameState->GetCurrentTurnPhase() == ENBTurnPhase::StartingTurn)
		{
			DisplayedInputCount = 0;
			OnTurnStarted();
		}
	}
}

void UNBNumberPadWidget::NativeDestruct()
{
	InputValuesChangedListenerHandle.Unregister();
	TurnPhaseChangedListenerHandle.Unregister();
	RoundStateChangedListenerHandle.Unregister();
	GuessResultChangedListenerHandle.Unregister();
	PendingTaskChangedListenerHandle.Unregister();

	Super::NativeDestruct();
}

void UNBNumberPadWidget::HandleInputValuesChanged(
	FGameplayTag Channel,
	const FNBInputValuesChangedMessage& Message)
{
	ApplyInputValues(Message.InputValues);
}

void UNBNumberPadWidget::HandleTurnPhaseChanged(
	FGameplayTag Channel,
	const FNBTurnPhaseChangedMessage& Message)
{
	if (Message.TurnPhase == ENBTurnPhase::StartingTurn)
	{
		DisplayedInputCount = 0;
		OnTurnStarted();
	}
}

void UNBNumberPadWidget::HandleRoundStateChanged(
	FGameplayTag Channel,
	const FNBRoundStateChangedMessage& Message)
{
	if (Message.RoundPhase == ENBRoundPhase::Starting)
	{
		OnRoundStarted(
			Message.CurrentRound,
			Message.TotalRoundCount,
			Message.RequiredInputCount);
	}
	else if (Message.RoundPhase == ENBRoundPhase::Ended)
	{
		OnRoundEnded(Message.CurrentRound);
	}
}

void UNBNumberPadWidget::HandleGuessResultChanged(
	FGameplayTag Channel,
	const FNBGuessResult& GuessResult)
{
	ApplyGuessResult(GuessResult);
}

void UNBNumberPadWidget::HandlePendingTaskChanged(
	FGameplayTag Channel,
	const FNBPendingTaskState& PendingTaskState)
{
	if (PendingTaskState.TaskTag != NBPendingTaskTags::NumberPadResult)
	{
		return;
	}

	ActivePendingTaskId = PendingTaskState.bIsActive
		? PendingTaskState.TaskId
		: INDEX_NONE;
}

void UNBNumberPadWidget::ApplyInputValues(const TArray<int32>& InputValues)
{
	if (InputValues.Num() < DisplayedInputCount)
	{
		DisplayedInputCount = 0;
	}

	for (int32 Index = DisplayedInputCount; Index < InputValues.Num(); ++Index)
	{
		OnInputValueAdded(InputValues[Index]);
	}

	DisplayedInputCount = InputValues.Num();
}

void UNBNumberPadWidget::ApplyGuessResult(const FNBGuessResult& GuessResult)
{
	TArray<ENBNumberMatchResult> GuessResults;
	GuessResults.Reserve(GuessResult.NumberResults.Num());

	for (const FNBNumberResult& NumberResult : GuessResult.NumberResults)
	{
		GuessResults.Add(NumberResult.MatchResult);
	}

	OnGuessResultsUpdated(GuessResults);
}
