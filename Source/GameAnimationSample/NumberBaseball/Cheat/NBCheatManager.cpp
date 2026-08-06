#include "NBCheatManager.h"

#include "EngineUtils.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
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

void UNBCheatManager::PrintNetworkPlayers()
{
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogNBCheat, Warning, TEXT("World not found"));
		return;
	}

	int32 PawnCount = 0;
	int32 PlayerControllerCount = 0;
	for (TActorIterator<APlayerController> It(World); It; ++It)
	{
		APlayerController* PlayerController = *It;
		const FString ControllerName = GetNameSafe(PlayerController);
		const FString PlayerStateName = GetNameSafe(PlayerController->PlayerState.Get());
		const FString PawnName = GetNameSafe(PlayerController->GetPawn());
		++PlayerControllerCount;
		UE_LOG(LogNBCheat, Display,
			TEXT("PlayerController: Name=%s PlayerState=%s Pawn=%s Local=%s LocalRole=%d RemoteRole=%d"),
			*ControllerName,
			*PlayerStateName,
			*PawnName,
			PlayerController->IsLocalController() ? TEXT("true") : TEXT("false"),
			static_cast<int32>(PlayerController->GetLocalRole()),
			static_cast<int32>(PlayerController->GetRemoteRole()));
	}

	for (TActorIterator<APawn> It(World); It; ++It)
	{
		const APawn* Pawn = *It;
		const FString PawnName = GetNameSafe(Pawn);
		const FString ControllerName = GetNameSafe(Pawn->GetController());
		const FString OwnerName = GetNameSafe(Pawn->GetOwner());
		++PawnCount;
		UE_LOG(LogNBCheat, Display,
			TEXT("Pawn: Name=%s Controller=%s Owner=%s LocalRole=%d RemoteRole=%d Replicates=%s Hidden=%s"),
			*PawnName,
			*ControllerName,
			*OwnerName,
			static_cast<int32>(Pawn->GetLocalRole()),
			static_cast<int32>(Pawn->GetRemoteRole()),
			Pawn->GetIsReplicated() ? TEXT("true") : TEXT("false"),
			Pawn->IsHidden() ? TEXT("true") : TEXT("false"));
	}

	const FString Message = FString::Printf(
		TEXT("NetworkPlayers: NetMode=%d PlayerControllerCount=%d PawnCount=%d"),
		static_cast<int32>(World->GetNetMode()),
		PlayerControllerCount,
		PawnCount);
	UE_LOG(LogNBCheat, Display, TEXT("%s"), *Message);

	if (APlayerController* PlayerController = GetPlayerController())
	{
		PlayerController->ClientMessage(Message);

		if (World->GetNetMode() == NM_Client)
		{
			PlayerController->ServerExec(TEXT("PrintNetworkPlayers"));
		}
	}
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
