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

	int32 GetPlayerCount() const;
	APlayerState* GetPlayerAt(int32 Index) const;
	int32 FindPlayerIndex(const APlayerState* PlayerState) const;

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<APlayerState>> ConnectedPlayers;
};
