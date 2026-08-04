#include "NBPlayerRegistryComponent.h"

#include "GameFramework/PlayerState.h"

void UNBPlayerRegistryComponent::AddPlayer(APlayerState* PlayerState)
{
	if (IsValid(PlayerState))
	{
		ConnectedPlayers.Add(PlayerState);
	}
}

void UNBPlayerRegistryComponent::RemovePlayer(APlayerState* PlayerState)
{
	if (IsValid(PlayerState))
	{
		ReadyPlayers.Remove(PlayerState);
		ConnectedPlayers.Remove(PlayerState);
	}
}

void UNBPlayerRegistryComponent::SetPlayerReady(APlayerState* PlayerState, bool bIsReady)
{
	if (IsValid(PlayerState) == false || ConnectedPlayers.Contains(PlayerState) == false)
	{
		return;
	}

	if (bIsReady)
	{
		ReadyPlayers.Add(PlayerState);
	}
	else
	{
		ReadyPlayers.Remove(PlayerState);
	}
}

int32 UNBPlayerRegistryComponent::GetPlayerCount() const
{
	return ConnectedPlayers.Num();
}

bool UNBPlayerRegistryComponent::AreAllPlayersReady(int32 MinimumPlayerCount) const
{
	return ConnectedPlayers.Num() >= MinimumPlayerCount && ReadyPlayers.Num() == ConnectedPlayers.Num();
}
