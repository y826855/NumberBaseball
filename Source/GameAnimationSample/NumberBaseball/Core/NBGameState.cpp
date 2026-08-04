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
	DOREPLIFETIME(ThisClass, RequiredInputCount);
	DOREPLIFETIME(ThisClass, CurrentRound);
	DOREPLIFETIME(ThisClass, TotalRoundCount);
	DOREPLIFETIME(ThisClass, UserInputEndServerTime);
	DOREPLIFETIME(ThisClass, bLastAnswerCorrect);
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

void ANBGameState::SetLastAnswerCorrect(bool bIsCorrect)
{
	if (HasAuthority())
	{
		bLastAnswerCorrect = bIsCorrect;
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
