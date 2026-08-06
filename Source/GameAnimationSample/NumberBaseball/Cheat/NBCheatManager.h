#pragma once

#include "GameFramework/CheatManager.h"
#include "NBCheatManager.generated.h"

UCLASS()
class GAMEANIMATIONSAMPLE_API UNBCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	UFUNCTION(Exec)
	void ForceStartGame();

	UFUNCTION(Exec)
	void PrintGameState();

	UFUNCTION(Exec)
	void PrintNetworkPlayers();

	UFUNCTION(Exec)
	void ForceUserInputTurn();
};
