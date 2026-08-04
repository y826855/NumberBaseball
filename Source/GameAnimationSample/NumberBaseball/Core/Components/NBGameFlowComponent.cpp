#include "NBGameFlowComponent.h"

#include "Engine/World.h"
#include "GameAnimationSample/NumberBaseball/Core/NBGameState.h"
#include "GameAnimationSample/NumberBaseball/Struct/NBGameplayMessages.h"

UNBGameFlowComponent::UNBGameFlowComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UNBGameFlowComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ANBGameState* NBGameState = GetNBGameState())
	{
		NBGameState->SetRoundState(0, TotalRoundCount);
	}

	InputValuesChangedListenerHandle = UGameplayMessageSubsystem::Get(this)
		.RegisterListener<FNBInputValuesChangedMessage>(
			NBGameplayMessages::InputValuesChanged,
			this,
			&ThisClass::HandleInputValuesChanged);
}

void UNBGameFlowComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(UserInputTimerHandle);

	InputValuesChangedListenerHandle.Unregister();

	Super::EndPlay(EndPlayReason);
}

void UNBGameFlowComponent::StartGameFlow()
{
	ANBGameState* NBGameState = GetNBGameState();
	if (IsValid(NBGameState) == false)
	{
		return;
	}

	CurrentRound = 0;
	bLastAnswerCorrect = false;
	AnswerNumbers.Reset();
	GetWorld()->GetTimerManager().ClearTimer(UserInputTimerHandle);
	NBGameState->ClearInputValues();
	NBGameState->SetUserInputEndServerTime(0.f);
	NBGameState->SetLastAnswerCorrect(false);
	NBGameState->SetRoundState(CurrentRound, TotalRoundCount);
	NBGameState->SetCurrentTurnPhase(ENBTurnPhase::StartingTurn);
	NBGameState->SetCurrentGamePhase(ENBGamePhase::Intro);
}

void UNBGameFlowComponent::StartRound()
{
	ANBGameState* NBGameState = GetNBGameState();
	if (IsValid(NBGameState) == false)
	{
		return;
	}

	if (CurrentRound >= TotalRoundCount)
	{
		NBGameState->SetCurrentGamePhase(ENBGamePhase::Outro);
		return;
	}

	++CurrentRound;
	GenerateAnswerNumbers();

	NBGameState->ClearInputValues();
	NBGameState->SetRequiredInputCount(AnswerNumbers.Num());
	NBGameState->SetRoundState(CurrentRound, TotalRoundCount);
	NBGameState->SetCurrentGamePhase(ENBGamePhase::Playing);
	NBGameState->SetCurrentTurnPhase(ENBTurnPhase::StartingTurn);
}

void UNBGameFlowComponent::StartUserInputTurn()
{
	ANBGameState* NBGameState = GetNBGameState();
	if (IsValid(NBGameState)
		&& NBGameState->GetCurrentTurnPhase() == ENBTurnPhase::StartingTurn)
	{
		bLastAnswerCorrect = false;
		NBGameState->SetLastAnswerCorrect(false);
		NBGameState->SetUserInputEndServerTime(
			NBGameState->GetServerWorldTimeSeconds() + UserInputDuration);
		NBGameState->SetCurrentTurnPhase(ENBTurnPhase::UserInputTurn);
		GetWorld()->GetTimerManager().SetTimer(
			UserInputTimerHandle,
			this,
			&ThisClass::EndUserInputTurn,
			UserInputDuration,
			false);
	}
}

void UNBGameFlowComponent::EndUserInputTurn()
{
	ANBGameState* NBGameState = GetNBGameState();
	if (IsValid(NBGameState) == false
		|| NBGameState->GetCurrentTurnPhase() != ENBTurnPhase::UserInputTurn)
	{
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(UserInputTimerHandle);
	bLastAnswerCorrect = AnswerNumbers == NBGameState->GetCurrentInputValues();
	NBGameState->SetUserInputEndServerTime(0.f);
	NBGameState->SetLastAnswerCorrect(bLastAnswerCorrect);
	NBGameState->SetCurrentTurnPhase(ENBTurnPhase::EndingTurn);
}

void UNBGameFlowComponent::CompleteTurn()
{
	ANBGameState* NBGameState = GetNBGameState();
	if (IsValid(NBGameState) == false
		|| NBGameState->GetCurrentTurnPhase() != ENBTurnPhase::EndingTurn)
	{
		return;
	}

	if (bLastAnswerCorrect)
	{
		CompleteRound();
		return;
	}

	NBGameState->ClearInputValues();
	NBGameState->SetCurrentTurnPhase(ENBTurnPhase::StartingTurn);
}

void UNBGameFlowComponent::CompleteRound()
{
	if (CurrentRound >= TotalRoundCount)
	{
		if (ANBGameState* NBGameState = GetNBGameState())
		{
			NBGameState->SetCurrentGamePhase(ENBGamePhase::Outro);
		}

		return;
	}

	StartRound();
}

void UNBGameFlowComponent::FinishGameFlow()
{
	GetWorld()->GetTimerManager().ClearTimer(UserInputTimerHandle);

	if (ANBGameState* NBGameState = GetNBGameState())
	{
		NBGameState->SetUserInputEndServerTime(0.f);
		NBGameState->SetCurrentGamePhase(ENBGamePhase::Finished);
	}
}

void UNBGameFlowComponent::ForceStartGameFlow()
{
	StartGameFlow();
	StartRound();
	StartUserInputTurn();
}

ANBGameState* UNBGameFlowComponent::GetNBGameState() const
{
	UWorld* World = GetWorld();
	return IsValid(World) ? World->GetGameState<ANBGameState>() : nullptr;
}

void UNBGameFlowComponent::GenerateAnswerNumbers()
{
	TArray<int32> AvailableNumbers;
	for (int32 Number = 1; Number <= 9; ++Number)
	{
		AvailableNumbers.Add(Number);

		UE_LOG(LogTemp, Error, TEXT("%d"), Number);
	}

	AnswerNumbers.Reset();
	const int32 Count = FMath::Clamp(AnswerNumberCount, 1, AvailableNumbers.Num());
	for (int32 Index = 0; Index < Count; ++Index)
	{
		const int32 RandomIndex = FMath::RandRange(0, AvailableNumbers.Num() - 1);
		AnswerNumbers.Add(AvailableNumbers[RandomIndex]);
		AvailableNumbers.RemoveAtSwap(RandomIndex);
	}
}

void UNBGameFlowComponent::HandleInputValuesChanged(
	FGameplayTag Channel,
	const FNBInputValuesChangedMessage& Message)
{
	ANBGameState* NBGameState = GetNBGameState();
	if (IsValid(NBGameState)
		&& NBGameState->GetCurrentTurnPhase() == ENBTurnPhase::UserInputTurn
		&& Message.RequiredInputCount > 0
		&& Message.InputValues.Num() >= Message.RequiredInputCount)
	{
		EndUserInputTurn();
	}
}
