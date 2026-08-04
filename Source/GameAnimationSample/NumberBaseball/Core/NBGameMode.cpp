#include "NBGameMode.h"

#include "Components/NBGameFlowComponent.h"
#include "Components/NBPlayerRegistryComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "NBGameState.h"

ANBGameMode::ANBGameMode()
{
	bDelayedStart = false;
	GameStateClass = ANBGameState::StaticClass();

	PlayerRegistryComponent = CreateDefaultSubobject<UNBPlayerRegistryComponent>(TEXT("PlayerRegistryComponent"));
	GameFlowComponent = CreateDefaultSubobject<UNBGameFlowComponent>(TEXT("GameFlowComponent"));
}

void ANBGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (IsValid(NewPlayer))
	{
		PlayerRegistryComponent->AddPlayer(NewPlayer->GetPlayerState<APlayerState>());
		EvaluateGamePhase();
	}
}

void ANBGameMode::Logout(AController* Exiting)
{
	if (IsValid(Exiting))
	{
		PlayerRegistryComponent->RemovePlayer(Exiting->GetPlayerState<APlayerState>());
		EvaluateGamePhase();
	}

	Super::Logout(Exiting);
}

void ANBGameMode::SetPlayerReady(APlayerState* PlayerState, bool bIsReady)
{
	PlayerRegistryComponent->SetPlayerReady(PlayerState, bIsReady);
	EvaluateGamePhase();
}

void ANBGameMode::ForceStartGame()
{
	GetWorldTimerManager().ClearTimer(GameStartTimerHandle);
	GameFlowComponent->ForceStartGameFlow();
}

void ANBGameMode::EvaluateGamePhase()
{
	ANBGameState* NBGameState = GetGameState<ANBGameState>();
	if (IsValid(NBGameState) == false)
	{
		return;
	}

	if (NBGameState->GetCurrentGamePhase() == ENBGamePhase::WaitingForReady)
	{
		if (PlayerRegistryComponent->AreAllPlayersReady(MinimumPlayerCount))
		{
			if (GetWorldTimerManager().IsTimerActive(GameStartTimerHandle) == false)
			{
				GetWorldTimerManager().SetTimer(
					GameStartTimerHandle,
					this,
					&ThisClass::HandleGameStartDelayExpired,
					GameStartDelay,
					false);
			}
		}
		else
		{
			GetWorldTimerManager().ClearTimer(GameStartTimerHandle);
		}

		return;
	}

	if (NBGameState->GetCurrentGamePhase() != ENBGamePhase::Finished
		&& PlayerRegistryComponent->GetPlayerCount() < MinimumPlayerCount)
	{
		GetWorldTimerManager().ClearTimer(GameStartTimerHandle);
		GameFlowComponent->FinishGameFlow();
	}
}

void ANBGameMode::HandleGameStartDelayExpired()
{
	ANBGameState* NBGameState = GetGameState<ANBGameState>();
	if (IsValid(NBGameState)
		&& NBGameState->GetCurrentGamePhase() == ENBGamePhase::WaitingForReady
		&& PlayerRegistryComponent->AreAllPlayersReady(MinimumPlayerCount))
	{
		GameFlowComponent->StartGameFlow();
	}
}
