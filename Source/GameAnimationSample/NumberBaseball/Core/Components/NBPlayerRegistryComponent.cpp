#include "NBPlayerRegistryComponent.h"

#include "GameFramework/PlayerState.h"

void UNBPlayerRegistryComponent::AddPlayer(APlayerState* PlayerState)
{
	if (IsValid(PlayerState))
	{
		ConnectedPlayers.AddUnique(PlayerState);
	}
}

void UNBPlayerRegistryComponent::RemovePlayer(APlayerState* PlayerState)
{
	if (IsValid(PlayerState))
	{
		ConnectedPlayers.Remove(PlayerState);
	}
}

int32 UNBPlayerRegistryComponent::GetPlayerCount() const
{
	return ConnectedPlayers.Num();
}

APlayerState* UNBPlayerRegistryComponent::GetPlayerAt(int32 Index) const
{
	return ConnectedPlayers.IsValidIndex(Index) ? ConnectedPlayers[Index] : nullptr;
}

APlayerState* UNBPlayerRegistryComponent::GetNextPlayer(
	const APlayerState* PlayerState) const
{
	const int32 PlayerIndex = FindPlayerIndex(PlayerState);
	if (PlayerIndex == INDEX_NONE || ConnectedPlayers.Num() <= 1)
	{
		return nullptr;
	}

	const int32 NextIndex = (PlayerIndex + 1) % ConnectedPlayers.Num();
	return ConnectedPlayers[NextIndex];
}

int32 UNBPlayerRegistryComponent::FindPlayerIndex(const APlayerState* PlayerState) const
{
	return ConnectedPlayers.IndexOfByKey(PlayerState);
}
