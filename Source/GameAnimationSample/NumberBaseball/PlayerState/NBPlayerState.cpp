#include "NBPlayerState.h"

#include "Engine/World.h"
#include "GameAnimationSample/NumberBaseball/Core/NBGameState.h"
#include "GameAnimationSample/NumberBaseball/Struct/NBGameplayMessages.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Net/UnrealNetwork.h"

void ANBPlayerState::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CorrectAnswerCount);
}

void ANBPlayerState::AddCorrectAnswer()
{
	if (HasAuthority())
	{
		++CorrectAnswerCount;
		OnRep_CorrectAnswerCount();
	}
}

void ANBPlayerState::ResetCorrectAnswerCount()
{
	if (HasAuthority() && CorrectAnswerCount != 0)
	{
		CorrectAnswerCount = 0;
		OnRep_CorrectAnswerCount();
	}
}

void ANBPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();

	if (ANBGameState* NBGameState = GetWorld()->GetGameState<ANBGameState>())
	{
		NBGameState->NotifyPlayerInfoReady(this);
	}
}

void ANBPlayerState::OnRep_CorrectAnswerCount()
{
	FNBScoreChangedMessage Message;
	Message.PlayerState = this;
	Message.CorrectAnswerCount = CorrectAnswerCount;
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(
		NBGameplayMessages::ScoreChanged,
		Message);
}
