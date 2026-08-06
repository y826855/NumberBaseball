#include "NBNumberPadWidget.h"

#include "Components/PanelWidget.h"
#include "Engine/World.h"
#include "GameAnimationSample/NumberBaseball/Core/NBGameState.h"
#include "GameAnimationSample/NumberBaseball/PlayerController/Components/NBPendingTaskClientComponent.h"
#include "GameAnimationSample/NumberBaseball/PlayerController/NBPlayerController.h"
#include "GameAnimationSample/NumberBaseball/Struct/NBGameplayMessages.h"
#include "GameAnimationSample/NumberBaseball/Struct/NBPendingTask.h"
#include "NBNumberWidget.h"
#include "NBResultWidget.h"
#include "TimerManager.h"

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
		if (NBGameState->GetCurrentRoundPhase() == ENBRoundPhase::Starting
			|| NBGameState->GetCurrentRoundPhase() == ENBRoundPhase::InProgress)
		{
			CreateRoundWidgets(NBGameState->GetRequiredInputCount());
		}
		else if (NBGameState->GetCurrentRoundPhase() == ENBRoundPhase::Ended)
		{
			ClearRoundWidgets();
		}

		if (NBGameState->GetCurrentRoundPhase() == ENBRoundPhase::InProgress
			&& NBGameState->GetCurrentTurnPhase() == ENBTurnPhase::StartingTurn)
		{
			ResetTurnWidgets();
		}

		HandlePendingTaskChanged(FGameplayTag(), NBGameState->GetPendingTaskState());
		ApplyInputValues(NBGameState->GetCurrentInputValues());
		ApplyGuessResult(NBGameState->GetLastGuessResult());
	}
}

void UNBNumberPadWidget::NativeDestruct()
{
	InputValuesChangedListenerHandle.Unregister();
	TurnPhaseChangedListenerHandle.Unregister();
	RoundStateChangedListenerHandle.Unregister();
	GuessResultChangedListenerHandle.Unregister();
	PendingTaskChangedListenerHandle.Unregister();
	GetWorld()->GetTimerManager().ClearTimer(ResultPresentationTimerHandle);

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
		ResetTurnWidgets();
	}
}

void UNBNumberPadWidget::HandleRoundStateChanged(
	FGameplayTag Channel,
	const FNBRoundStateChangedMessage& Message)
{
	if (Message.RoundPhase == ENBRoundPhase::Starting)
	{
		CreateRoundWidgets(Message.RequiredInputCount);
	}
	else if (Message.RoundPhase == ENBRoundPhase::Ended)
	{
		ClearRoundWidgets();
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

void UNBNumberPadWidget::CreateRoundWidgets(int32 RequiredInputCount)
{
	ClearRoundWidgets();

	if (IsValid(UpperBox) == false || IsValid(LowerBox) == false
		|| NumberWidgetClass == nullptr || ResultWidgetClass == nullptr)
	{
		return;
	}

	for (int32 Index = 0; Index < RequiredInputCount; ++Index)
	{
		UNBNumberWidget* NumberWidget = CreateWidget<UNBNumberWidget>(GetOwningPlayer(), NumberWidgetClass);
		UNBResultWidget* ResultWidget = CreateWidget<UNBResultWidget>(GetOwningPlayer(), ResultWidgetClass);
		if (IsValid(NumberWidget) == false || IsValid(ResultWidget) == false)
		{
			continue;
		}

		NumberWidget->SetVisibility(ESlateVisibility::Collapsed);
		ResultWidget->SetVisibility(ESlateVisibility::Collapsed);
		UpperBox->AddChild(NumberWidget);
		LowerBox->AddChild(ResultWidget);
		NumberWidgets.Add(NumberWidget);
		ResultWidgets.Add(ResultWidget);
	}
}

void UNBNumberPadWidget::ClearRoundWidgets()
{
	GetWorld()->GetTimerManager().ClearTimer(ResultPresentationTimerHandle);
	NumberWidgets.Reset();
	ResultWidgets.Reset();
	DisplayedInputCount = 0;

	if (IsValid(UpperBox))
	{
		UpperBox->ClearChildren();
	}

	if (IsValid(LowerBox))
	{
		LowerBox->ClearChildren();
	}
}

void UNBNumberPadWidget::ResetTurnWidgets()
{
	GetWorld()->GetTimerManager().ClearTimer(ResultPresentationTimerHandle);
	DisplayedInputCount = 0;

	for (UNBNumberWidget* NumberWidget : NumberWidgets)
	{
		if (IsValid(NumberWidget))
		{
			NumberWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	for (UNBResultWidget* ResultWidget : ResultWidgets)
	{
		if (IsValid(ResultWidget))
		{
			ResultWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UNBNumberPadWidget::ApplyInputValues(const TArray<int32>& InputValues)
{
	if (InputValues.Num() < DisplayedInputCount)
	{
		DisplayedInputCount = 0;
	}

	for (int32 Index = DisplayedInputCount; Index < InputValues.Num(); ++Index)
	{
		if (NumberWidgets.IsValidIndex(Index) && IsValid(NumberWidgets[Index]))
		{
			NumberWidgets[Index]->SetNumber(InputValues[Index]);
			NumberWidgets[Index]->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}

	DisplayedInputCount = InputValues.Num();
}

void UNBNumberPadWidget::ApplyGuessResult(const FNBGuessResult& GuessResult)
{
	for (int32 Index = 0; Index < GuessResult.NumberResults.Num(); ++Index)
	{
		if (ResultWidgets.IsValidIndex(Index) && IsValid(ResultWidgets[Index]))
		{
			ResultWidgets[Index]->ShowResult(GuessResult.NumberResults[Index].MatchResult);
		}
	}

	if (GuessResult.NumberResults.IsEmpty() == false)
	{
		GetWorld()->GetTimerManager().SetTimer(
			ResultPresentationTimerHandle,
			this,
			&ThisClass::FinishResultPresentation,
			ResultPresentationDuration,
			false);
	}
}
