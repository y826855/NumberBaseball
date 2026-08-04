#pragma once

#include "Components/ActorComponent.h"
#include "NBPlayerRegistryComponent.generated.h"

class APlayerState;

UCLASS()
class GAMEANIMATIONSAMPLE_API UNBPlayerRegistryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void AddPlayer(APlayerState* PlayerState);
	void RemovePlayer(APlayerState* PlayerState);
	void SetPlayerReady(APlayerState* PlayerState, bool bIsReady);

	int32 GetPlayerCount() const;
	bool AreAllPlayersReady(int32 MinimumPlayerCount) const;

private:
	UPROPERTY(Transient)
	TSet<TObjectPtr<APlayerState>> ConnectedPlayers;

	UPROPERTY(Transient)
	TSet<TObjectPtr<APlayerState>> ReadyPlayers;
};
