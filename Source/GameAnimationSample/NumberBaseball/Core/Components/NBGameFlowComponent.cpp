#include "NBGameFlowComponent.h"

#include "Engine/World.h"
#include "GameAnimationSample/NumberBaseball/Core/Components/NBPendingTaskComponent.h"
#include "GameAnimationSample/NumberBaseball/Core/Components/NBPlayerRegistryComponent.h"
#include "GameAnimationSample/NumberBaseball/Core/NBGameMode.h"
#include "GameAnimationSample/NumberBaseball/Core/NBGameState.h"
#include "GameAnimationSample/NumberBaseball/PlayerController/Components/NBClientMessageComponent.h"
#include "GameAnimationSample/NumberBaseball/PlayerController/NBPlayerController.h"
#include "GameAnimationSample/NumberBaseball/PlayerState/NBPlayerState.h"
#include "GameAnimationSample/NumberBaseball/Struct/NBGuessResult.h"
#include "GameAnimationSample/NumberBaseball/Struct/NBClientMessages.h"
#include "GameAnimationSample/NumberBaseball/Struct/NBGameplayMessages.h"
#include "GameAnimationSample/NumberBaseball/Struct/NBPendingTask.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"

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
	ClearFlowTimer();

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
	NextAttemptId = 0;
	bLastAnswerCorrect = false;
	AnswerNumbers.Reset();
	ClearFlowTimer();
	SendTurnNotifications(false);
	ResetPlayerScores();
	NBGameState->ClearInputValues();
	NBGameState->SetUserInputEndServerTime(0.f);
	NBGameState->SetLastGuessResult(FNBGuessResult());
	NBGameState->SetRoundState(CurrentRound, TotalRoundCount);
	NBGameState->SetCurrentRoundPhase(ENBRoundPhase::None);
	NBGameState->SetCurrentTurnPlayer(nullptr);
	NBGameState->SetCurrentGamePhase(ENBGamePhase::Intro);

	StartNextRound();
}

void UNBGameFlowComponent::StartNextRound()
{
	if (CurrentRound >= TotalRoundCount)
	{
		CompleteGameFlow();
		return;
	}

	StartRound();
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
		CompleteGameFlow();
		return;
	}

	if (SelectRoundStartingPlayer() == false)
	{
		return;
	}

	++CurrentRound;
	GenerateAnswerNumbers();

	NBGameState->ClearInputValues();
	NBGameState->SetRequiredInputCount(AnswerNumbers.Num());
	NBGameState->SetRoundState(CurrentRound, TotalRoundCount);
	NBGameState->SetCurrentRoundPhase(ENBRoundPhase::Starting);
	ScheduleFlowStep(RoundStartingDuration, &ThisClass::BeginRound);
}

void UNBGameFlowComponent::BeginRound()
{
	ANBGameState* NBGameState = GetNBGameState();
	if (IsValid(NBGameState) == false
		|| NBGameState->GetCurrentRoundPhase() != ENBRoundPhase::Starting)
	{
		return;
	}

	ClearFlowTimer();
	NBGameState->SetCurrentRoundPhase(ENBRoundPhase::InProgress);
	NBGameState->SetCurrentGamePhase(ENBGamePhase::Playing);
	StartTurn();
}

void UNBGameFlowComponent::StartTurn()
{
	ANBGameState* NBGameState = GetNBGameState();
	if (IsValid(NBGameState) == false
		|| NBGameState->GetCurrentRoundPhase() != ENBRoundPhase::InProgress)
	{
		return;
	}

	NBGameState->ClearInputValues();
	NBGameState->SetCurrentTurnPhase(ENBTurnPhase::StartingTurn);
	SendTurnNotifications(true);
	ScheduleFlowStep(StartingTurnDuration, &ThisClass::StartUserInputTurn);
}

void UNBGameFlowComponent::StartUserInputTurn()
{
	ANBGameState* NBGameState = GetNBGameState();
	if (IsValid(NBGameState)
		&& NBGameState->GetCurrentRoundPhase() == ENBRoundPhase::InProgress
		&& NBGameState->GetCurrentTurnPhase() == ENBTurnPhase::StartingTurn)
	{
		ClearFlowTimer();
		bLastAnswerCorrect = false;
		NBGameState->SetLastGuessResult(FNBGuessResult());
		NBGameState->SetUserInputEndServerTime(
			NBGameState->GetServerWorldTimeSeconds() + UserInputDuration);
		NBGameState->SetCurrentTurnPhase(ENBTurnPhase::UserInputTurn);
		SendUserInputTimer(true);
		ScheduleFlowStep(UserInputDuration, &ThisClass::EndUserInputTurn);
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

	ClearFlowTimer();
	const FNBGuessResult GuessResult = BuildGuessResult(NBGameState->GetCurrentInputValues());
	bLastAnswerCorrect = GuessResult.NumberResults.Num() == AnswerNumbers.Num()
		&& GuessResult.StrikeCount == AnswerNumbers.Num();
	NBGameState->SetUserInputEndServerTime(0.f);
	NBGameState->SetCurrentTurnPhase(ENBTurnPhase::EndingTurn);
	SendUserInputTimer(false);
	SendTurnNotifications(false);

	ANBGameMode* NBGameMode = Cast<ANBGameMode>(GetOwner());
	APlayerController* CompletionPlayer = GetWorld()->GetFirstPlayerController();
	UNBPendingTaskComponent* PendingTaskComponent = IsValid(NBGameMode)
		? NBGameMode->GetPendingTaskComponent()
		: nullptr;

	//비동기 턴 종료 연출 등록
	const bool bPendingTaskStarted = IsValid(PendingTaskComponent)
		&& PendingTaskComponent->StartPendingTask(
			NBPendingTaskTags::NumberPadResult,
			CompletionPlayer,
			ResultPresentationMaxWaitTime,
			FSimpleDelegate::CreateUObject(this, &ThisClass::CompleteTurn));

	NBGameState->SetLastGuessResult(GuessResult);

	if (bPendingTaskStarted)
	{
		return;
	}

	CompleteTurn();
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
		if (ANBPlayerState* PlayerState = Cast<ANBPlayerState>(
			NBGameState->GetCurrentTurnPlayer()))
		{
			PlayerState->AddCorrectAnswer();
		}
		CompleteRound();
		return;
	}

	if (AdvanceTurnPlayer() == false)
	{
		return;
	}

	StartTurn();
}

void UNBGameFlowComponent::CompleteRound()
{
	ANBGameState* NBGameState = GetNBGameState();
	if (IsValid(NBGameState) == false)
	{
		return;
	}

	ClearFlowTimer();
	NBGameState->SetUserInputEndServerTime(0.f);
	NBGameState->SetCurrentTurnPlayer(nullptr);
	NBGameState->SetCurrentRoundPhase(ENBRoundPhase::Ended);
	ScheduleFlowStep(RoundEndingDuration, &ThisClass::StartNextRound);
}

void UNBGameFlowComponent::CompleteGameFlow()
{
	ClearFlowTimer();

	if (ANBGameState* NBGameState = GetNBGameState())
	{
		NBGameState->SetUserInputEndServerTime(0.f);
		NBGameState->SetCurrentTurnPlayer(nullptr);
		NBGameState->SetCurrentGamePhase(ENBGamePhase::Outro);
	}
}

void UNBGameFlowComponent::FinishGameFlow()
{
	ClearFlowTimer();
	SendUserInputTimer(false);
	SendTurnNotifications(false);

	if (ANBGameState* NBGameState = GetNBGameState())
	{
		NBGameState->SetUserInputEndServerTime(0.f);
		NBGameState->SetCurrentTurnPlayer(nullptr);
		if (CurrentRound > 0)
		{
			NBGameState->SetCurrentRoundPhase(ENBRoundPhase::Ended);
		}
		NBGameState->SetCurrentGamePhase(ENBGamePhase::Finished);
	}
}

void UNBGameFlowComponent::ReturnToWaitingForPlayers()
{
	ClearFlowTimer();
	SendUserInputTimer(false);
	SendTurnNotifications(false);

	ANBGameMode* NBGameMode = Cast<ANBGameMode>(GetOwner());
	if (IsValid(NBGameMode) && IsValid(NBGameMode->GetPendingTaskComponent()))
	{
		NBGameMode->GetPendingTaskComponent()->CancelPendingTask();
	}

	CurrentRound = 0;
	NextAttemptId = 0;
	bLastAnswerCorrect = false;
	AnswerNumbers.Reset();
	ResetPlayerScores();

	if (ANBGameState* NBGameState = GetNBGameState())
	{
		NBGameState->ClearInputValues();
		NBGameState->SetRequiredInputCount(0);
		NBGameState->SetUserInputEndServerTime(0.f);
		NBGameState->SetLastGuessResult(FNBGuessResult());
		NBGameState->SetCurrentTurnPlayer(nullptr);
		NBGameState->SetCurrentTurnPhase(ENBTurnPhase::StartingTurn);
		NBGameState->SetCurrentRoundPhase(ENBRoundPhase::None);
		NBGameState->SetRoundState(0, TotalRoundCount);
		NBGameState->SetCurrentGamePhase(ENBGamePhase::WaitingForReady);
	}
}

void UNBGameFlowComponent::ForceStartGameFlow()
{
	StartGameFlow();
}

ANBGameState* UNBGameFlowComponent::GetNBGameState() const
{
	UWorld* World = GetWorld();
	return IsValid(World) ? World->GetGameState<ANBGameState>() : nullptr;
}

void UNBGameFlowComponent::ScheduleFlowStep(
	float Delay,
	FFlowTimerCallback Callback)
{
	GetWorld()->GetTimerManager().SetTimer(
		FlowTimerHandle,
		this,
		Callback,
		Delay,
		false);
}

void UNBGameFlowComponent::ClearFlowTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(FlowTimerHandle);
}

void UNBGameFlowComponent::SendUserInputTimer(bool bIsTimerActive) const
{
	ANBGameMode* NBGameMode = Cast<ANBGameMode>(GetOwner());
	ANBGameState* NBGameState = GetNBGameState();
	if (IsValid(NBGameMode) == false || IsValid(NBGameState) == false)
	{
		return;
	}

	FNBTimerMessage Message;
	Message.TimerType = NBTimerTypes::UserInput;
	Message.Duration = UserInputDuration;
	Message.bIsActive = bIsTimerActive;
	if (bIsTimerActive)
	{
		Message.EndServerTime =
			NBGameState->GetServerWorldTimeSeconds() + UserInputDuration;
	}

	NBGameMode->BroadcastTimerMessage(Message);
}

void UNBGameFlowComponent::SendTurnNotifications(bool bIsTurnActive) const
{
	ANBGameState* NBGameState = GetNBGameState();
	if (IsValid(NBGameState) == false)
	{
		return;
	}

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator();
		Iterator;
		++Iterator)
	{
		ANBPlayerController* PlayerController = Cast<ANBPlayerController>(Iterator->Get());
		if (IsValid(PlayerController) == false
			|| IsValid(PlayerController->GetClientMessageComponent()) == false)
		{
			continue;
		}

		FNBNotifyMessage Message;
		Message.bIsActive = bIsTurnActive;
		if (bIsTurnActive)
		{
			const bool bIsTurnOwner = PlayerController->GetPlayerState<APlayerState>()
				== NBGameState->GetCurrentTurnPlayer();
			Message.MessageType = bIsTurnOwner
				? NBNotifyTypes::YourTurn
				: NBNotifyTypes::OpponentTurn;
			Message.Message = bIsTurnOwner
				? NSLOCTEXT("NumberBaseball", "YourTurnNotification", "버튼 위에 올라서서 숫자를 입력하세요.")
				: NSLOCTEXT("NumberBaseball", "OpponentTurnNotification", "조준하여 공을 발사해 숫자 입력을 저지하세요");
		}

		PlayerController->GetClientMessageComponent()->SendNotifyMessage(Message);
	}
}

void UNBGameFlowComponent::ResetPlayerScores() const
{
	const ANBGameMode* NBGameMode = Cast<ANBGameMode>(GetOwner());
	UNBPlayerRegistryComponent* PlayerRegistry = IsValid(NBGameMode)
		? NBGameMode->GetPlayerRegistryComponent()
		: nullptr;
	if (IsValid(PlayerRegistry) == false)
	{
		return;
	}

	for (int32 Index = 0; Index < PlayerRegistry->GetPlayerCount(); ++Index)
	{
		if (ANBPlayerState* PlayerState = Cast<ANBPlayerState>(
			PlayerRegistry->GetPlayerAt(Index)))
		{
			PlayerState->ResetCorrectAnswerCount();
		}
	}
}

bool UNBGameFlowComponent::SelectRoundStartingPlayer()
{
	ANBGameMode* NBGameMode = Cast<ANBGameMode>(GetOwner());
	ANBGameState* NBGameState = GetNBGameState();
	UNBPlayerRegistryComponent* PlayerRegistry = IsValid(NBGameMode)
		? NBGameMode->GetPlayerRegistryComponent()
		: nullptr;
	if (IsValid(NBGameState) == false
		|| IsValid(PlayerRegistry) == false
		|| PlayerRegistry->GetPlayerCount() == 0)
	{
		return false;
	}

	const int32 PlayerIndex = FMath::RandRange(0, PlayerRegistry->GetPlayerCount() - 1);
	NBGameState->SetCurrentTurnPlayer(PlayerRegistry->GetPlayerAt(PlayerIndex));
	return true;
}

bool UNBGameFlowComponent::AdvanceTurnPlayer()
{
	ANBGameMode* NBGameMode = Cast<ANBGameMode>(GetOwner());
	ANBGameState* NBGameState = GetNBGameState();
	UNBPlayerRegistryComponent* PlayerRegistry = IsValid(NBGameMode)
		? NBGameMode->GetPlayerRegistryComponent()
		: nullptr;
	if (IsValid(NBGameState) == false
		|| IsValid(PlayerRegistry) == false
		|| PlayerRegistry->GetPlayerCount() == 0)
	{
		return false;
	}

	const int32 CurrentIndex = PlayerRegistry->FindPlayerIndex(
		NBGameState->GetCurrentTurnPlayer());
	const int32 NextIndex = CurrentIndex == INDEX_NONE
		? 0
		: (CurrentIndex + 1) % PlayerRegistry->GetPlayerCount();
	NBGameState->SetCurrentTurnPlayer(PlayerRegistry->GetPlayerAt(NextIndex));
	return true;
}

void UNBGameFlowComponent::GenerateAnswerNumbers()
{
	TArray<int32> AvailableNumbers;
	for (int32 Number = 1; Number <= 9; ++Number)
	{
		AvailableNumbers.Add(Number);
	}

	AnswerNumbers.Reset();
	const int32 Count = FMath::Clamp(AnswerNumberCount, 1, AvailableNumbers.Num());
	for (int32 Index = 0; Index < Count; ++Index)
	{
		const int32 RandomIndex = FMath::RandRange(0, AvailableNumbers.Num() - 1);
		AnswerNumbers.Add(AvailableNumbers[RandomIndex]);
		UE_LOG(LogTemp, Error, TEXT("%d"), AvailableNumbers[RandomIndex]);
		AvailableNumbers.RemoveAtSwap(RandomIndex);
	}
}

FNBGuessResult UNBGameFlowComponent::BuildGuessResult(const TArray<int32>& InputValues)
{
	FNBGuessResult GuessResult;
	GuessResult.AttemptId = ++NextAttemptId;

	for (int32 Index = 0; Index < InputValues.Num(); ++Index)
	{
		FNBNumberResult NumberResult;
		NumberResult.Number = InputValues[Index];

		if (AnswerNumbers.IsValidIndex(Index)
			&& AnswerNumbers[Index] == NumberResult.Number)
		{
			NumberResult.MatchResult = ENBNumberMatchResult::Strike;
			++GuessResult.StrikeCount;
		}
		else if (AnswerNumbers.Contains(NumberResult.Number))
		{
			NumberResult.MatchResult = ENBNumberMatchResult::Ball;
			++GuessResult.BallCount;
		}

		GuessResult.NumberResults.Add(NumberResult);
	}

	return GuessResult;
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
