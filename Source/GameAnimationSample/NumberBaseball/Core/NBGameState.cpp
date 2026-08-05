#include "NBGameState.h"

#include "GameAnimationSample/NumberBaseball/Struct/NBGameplayMessages.h"
#include "Net/UnrealNetwork.h"

void ANBGameState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		InputSubmittedListenerHandle = UGameplayMessageSubsystem::Get(this).RegisterListener<FNBInputMessage>(
			NBGameplayMessages::InputSubmitted,
			this,
			&ThisClass::HandleInputSubmitted);
	}
}

void ANBGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	InputSubmittedListenerHandle.Unregister();
	Super::EndPlay(EndPlayReason);
}

void ANBGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentGamePhase);
	DOREPLIFETIME(ThisClass, CurrentTurnPhase);
	DOREPLIFETIME(ThisClass, CurrentInputValues);
	DOREPLIFETIME(ThisClass, CurrentRoundPhase);
	DOREPLIFETIME(ThisClass, RoundStateRevision);
	DOREPLIFETIME(ThisClass, RequiredInputCount);
	DOREPLIFETIME(ThisClass, CurrentRound);
	DOREPLIFETIME(ThisClass, TotalRoundCount);
	DOREPLIFETIME(ThisClass, UserInputEndServerTime);
	DOREPLIFETIME(ThisClass, LastGuessResult);
	DOREPLIFETIME(ThisClass, PendingTaskState);
}

void ANBGameState::SetCurrentGamePhase(ENBGamePhase NewGamePhase)
{
	if (HasAuthority() == false || CurrentGamePhase == NewGamePhase)
	{
		return;
	}

	CurrentGamePhase = NewGamePhase;
	OnRep_CurrentGamePhase();
}

void ANBGameState::SetCurrentTurnPhase(ENBTurnPhase NewTurnPhase)
{
	if (HasAuthority() == false || CurrentTurnPhase == NewTurnPhase)
	{
		return;
	}

	CurrentTurnPhase = NewTurnPhase;
	OnRep_CurrentTurnPhase();
}

void ANBGameState::SetCurrentRoundPhase(ENBRoundPhase NewRoundPhase)
{
	if (HasAuthority() == false || CurrentRoundPhase == NewRoundPhase)
	{
		return;
	}

	CurrentRoundPhase = NewRoundPhase;
	++RoundStateRevision;
	OnRep_CurrentRoundPhase();
}

void ANBGameState::AddInputValue(int32 Value)
{
	if (HasAuthority() == false
		|| CurrentTurnPhase != ENBTurnPhase::UserInputTurn
		|| RequiredInputCount <= CurrentInputValues.Num())
	{
		return;
	}

	CurrentInputValues.Add(Value);
	OnRep_CurrentInputValues();
}

void ANBGameState::SetRequiredInputCount(int32 Count)
{
	if (HasAuthority())
	{
		RequiredInputCount = FMath::Max(0, Count);
	}
}

void ANBGameState::SetRoundState(int32 NewCurrentRound, int32 NewTotalRoundCount)
{
	if (HasAuthority())
	{
		CurrentRound = NewCurrentRound;
		TotalRoundCount = NewTotalRoundCount;
	}
}

void ANBGameState::SetUserInputEndServerTime(float EndServerTime)
{
	if (HasAuthority())
	{
		UserInputEndServerTime = EndServerTime;
	}
}

void ANBGameState::SetLastGuessResult(const FNBGuessResult& NewGuessResult)
{
	if (HasAuthority())
	{
		LastGuessResult = NewGuessResult;
		OnRep_LastGuessResult();
	}
}

void ANBGameState::SetPendingTaskState(const FNBPendingTaskState& NewPendingTaskState)
{
	if (HasAuthority())
	{
		PendingTaskState = NewPendingTaskState;
		OnRep_PendingTaskState();
	}
}

float ANBGameState::GetRemainingUserInputTime() const
{
	if (CurrentTurnPhase != ENBTurnPhase::UserInputTurn)
	{
		return 0.f;
	}

	return FMath::Max(0.f, UserInputEndServerTime - GetServerWorldTimeSeconds());
}

void ANBGameState::HandleInputSubmitted(
	FGameplayTag Channel,
	const FNBInputMessage& Message)
{
	AddInputValue(Message.Value);
}

void ANBGameState::ClearInputValues()
{
	if (HasAuthority() == false || CurrentInputValues.IsEmpty())
	{
		return;
	}

	CurrentInputValues.Reset();
	OnRep_CurrentInputValues();
}

void ANBGameState::OnRep_CurrentGamePhase()
{
	FNBGamePhaseChangedMessage Message;
	Message.GamePhase = CurrentGamePhase;
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(
		NBGameplayMessages::GamePhaseChanged,
		Message);
}

void ANBGameState::OnRep_CurrentTurnPhase()
{
	FNBTurnPhaseChangedMessage Message;
	Message.TurnPhase = CurrentTurnPhase;
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(
		NBGameplayMessages::TurnPhaseChanged,
		Message);
}

void ANBGameState::OnRep_CurrentInputValues()
{
	FNBInputValuesChangedMessage Message;
	Message.InputValues = CurrentInputValues;
	Message.RequiredInputCount = RequiredInputCount;
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(
		NBGameplayMessages::InputValuesChanged,
		Message);
}

void ANBGameState::OnRep_CurrentRoundPhase()
{
	FNBRoundStateChangedMessage Message;
	Message.RoundPhase = CurrentRoundPhase;
	Message.CurrentRound = CurrentRound;
	Message.TotalRoundCount = TotalRoundCount;
	Message.RequiredInputCount = RequiredInputCount;
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(
		NBGameplayMessages::RoundStateChanged,
		Message);
}

void ANBGameState::OnRep_LastGuessResult()
{
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(
		NBGameplayMessages::GuessResultChanged,
		LastGuessResult);
}

void ANBGameState::OnRep_PendingTaskState()
{
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(
		NBGameplayMessages::PendingTaskChanged,
		PendingTaskState);
}
