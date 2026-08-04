#include "NBCheatManager.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameAnimationSample/NumberBaseball/Core/NBGameMode.h"
#include "GameAnimationSample/NumberBaseball/Core/NBGameState.h"

DEFINE_LOG_CATEGORY_STATIC(LogNBCheat, Log, All);

void UNBCheatManager::ForceStartGame()
{
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogNBCheat, Warning, TEXT("World not found"));
		return;
	}

	if (ANBGameMode* NBGameMode = Cast<ANBGameMode>(World->GetAuthGameMode()))
	{
		NBGameMode->ForceStartGame();
		UE_LOG(LogNBCheat, Display, TEXT("Game flow forced to UserInputTurn"));
		return;
	}

	if (World->GetNetMode() == NM_Client)
	{
		if (APlayerController* PlayerController = GetPlayerController())
		{
			PlayerController->ServerExec(TEXT("ForceStartGame"));
		}
	}
}

void UNBCheatManager::PrintGameState()
{
	APlayerController* PlayerController = GetPlayerController();
	UWorld* World = GetWorld();
	if (IsValid(PlayerController) == false || IsValid(World) == false)
	{
		return;
	}

	ANBGameState* NBGameState = World->GetGameState<ANBGameState>();
	if (IsValid(NBGameState) == false)
	{
		PlayerController->ClientMessage(TEXT("NBGameState not found"));
		UE_LOG(LogNBCheat, Warning, TEXT("NBGameState not found"));
		return;
	}

	const FString Message = FString::Printf(
		TEXT("GamePhase: %s | TurnPhase: %s | Input: %d/%d"),
		*UEnum::GetValueAsString(NBGameState->GetCurrentGamePhase()),
		*UEnum::GetValueAsString(NBGameState->GetCurrentTurnPhase()),
		NBGameState->GetCurrentInputValues().Num(),
		NBGameState->GetRequiredInputCount());

	PlayerController->ClientMessage(Message);
	UE_LOG(LogNBCheat, Display, TEXT("%s"), *Message);
}

void UNBCheatManager::ForceUserInputTurn()
{
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		return;
	}

	ANBGameState* NBGameState = World->GetGameState<ANBGameState>();
	if (IsValid(NBGameState) && NBGameState->HasAuthority())
	{
		NBGameState->SetCurrentTurnPhase(ENBTurnPhase::UserInputTurn);

		if (APlayerController* PlayerController = GetPlayerController())
		{
			PlayerController->ClientMessage(TEXT("TurnPhase changed to UserInputTurn"));
		}

		UE_LOG(LogNBCheat, Display, TEXT("TurnPhase changed to UserInputTurn"));
		return;
	}

	if (World->GetNetMode() == NM_Client)
	{
		if (APlayerController* PlayerController = GetPlayerController())
		{
			PlayerController->ServerExec(TEXT("ForceUserInputTurn"));
		}
	}
}
