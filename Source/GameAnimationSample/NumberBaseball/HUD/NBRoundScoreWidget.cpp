#include "NBRoundScoreWidget.h"

#include "Components/TextBlock.h"
#include "GameAnimationSample/NumberBaseball/Core/NBGameState.h"
#include "GameAnimationSample/NumberBaseball/PlayerState/NBPlayerState.h"
#include "GameAnimationSample/NumberBaseball/Struct/NBGameplayMessages.h"

void UNBRoundScoreWidget::NativeConstruct()
{
	Super::NativeConstruct();

	RoundStateChangedListenerHandle = UGameplayMessageSubsystem::Get(this)
		.RegisterListener<FNBRoundStateChangedMessage>(
			NBGameplayMessages::RoundStateChanged,
			this,
			&ThisClass::HandleRoundStateChanged);
	ScoreChangedListenerHandle = UGameplayMessageSubsystem::Get(this)
		.RegisterListener<FNBScoreChangedMessage>(
			NBGameplayMessages::ScoreChanged,
			this,
			&ThisClass::HandleScoreChanged);

	const ANBGameState* NBGameState = GetWorld()->GetGameState<ANBGameState>();
	UpdateRound(
		IsValid(NBGameState) ? NBGameState->GetCurrentRound() : 0,
		IsValid(NBGameState) ? NBGameState->GetTotalRoundCount() : 0);

	UpdateScores();
}

void UNBRoundScoreWidget::NativeDestruct()
{
	RoundStateChangedListenerHandle.Unregister();
	ScoreChangedListenerHandle.Unregister();
	Super::NativeDestruct();
}

void UNBRoundScoreWidget::UpdateRound(int32 CurrentRound, int32 TotalRoundCount)
{
	RoundText->SetText(FText::FromString(FString::Printf(
		TEXT("ROUND %d / %d"),
		CurrentRound,
		TotalRoundCount)));
	OnRoundUpdated(CurrentRound, TotalRoundCount);
}

void UNBRoundScoreWidget::UpdateScores()
{
	const ANBGameState* NBGameState = GetWorld()->GetGameState<ANBGameState>();
	FString ScoreLines;
	if (IsValid(NBGameState))
	{
		for (APlayerState* PlayerState : NBGameState->PlayerArray)
		{
			const ANBPlayerState* NBPlayerState = Cast<ANBPlayerState>(PlayerState);
			if (IsValid(NBPlayerState) == false
				|| NBPlayerState->GetPlayerName().IsEmpty())
			{
				continue;
			}

			if (ScoreLines.IsEmpty() == false)
			{
				ScoreLines.Append(LINE_TERMINATOR);
			}
			ScoreLines.Appendf(
				TEXT("%s : %d"),
				*NBPlayerState->GetPlayerName(),
				NBPlayerState->GetCorrectAnswerCount());
		}
	}

	ScoreText->SetText(FText::FromString(ScoreLines));
	OnScoresUpdated();
}

void UNBRoundScoreWidget::HandleRoundStateChanged(
	FGameplayTag Channel,
	const FNBRoundStateChangedMessage& Message)
{
	UpdateRound(Message.CurrentRound, Message.TotalRoundCount);
}

void UNBRoundScoreWidget::HandleScoreChanged(
	FGameplayTag Channel,
	const FNBScoreChangedMessage& Message)
{
	UpdateScores();
}
