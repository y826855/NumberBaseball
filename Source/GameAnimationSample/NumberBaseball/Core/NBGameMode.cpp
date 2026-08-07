#include "NBGameMode.h"

#include "Components/NBGameFlowComponent.h"
#include "Components/NBPendingTaskComponent.h"
#include "Components/NBPlayerRegistryComponent.h"
#include "Engine/NetDriver.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "NBGameState.h"
#include "GameAnimationSample/NumberBaseball/PlayerController/Components/NBClientMessageComponent.h"
#include "GameAnimationSample/NumberBaseball/PlayerController/NBPlayerController.h"
#include "GameAnimationSample/NumberBaseball/PlayerState/NBPlayerState.h"
#include "GameAnimationSample/NumberBaseball/Struct/NBClientMessages.h"
#include "GameAnimationSample/NumberBaseball/Struct/NBGameplayMessages.h"

DEFINE_LOG_CATEGORY_STATIC(LogNBGameMode, Log, All);

ANBGameMode::ANBGameMode()
{
	bDelayedStart = false;
	GameStateClass = ANBGameState::StaticClass();
	PlayerControllerClass = ANBPlayerController::StaticClass();
	PlayerStateClass = ANBPlayerState::StaticClass();

	PlayerRegistryComponent = CreateDefaultSubobject<UNBPlayerRegistryComponent>(TEXT("PlayerRegistryComponent"));
	GameFlowComponent = CreateDefaultSubobject<UNBGameFlowComponent>(TEXT("GameFlowComponent"));
	PendingTaskComponent = CreateDefaultSubobject<UNBPendingTaskComponent>(TEXT("PendingTaskComponent"));
}

void ANBGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogNBGameMode, Display,
		TEXT("Server ready: Map=%s NetMode=%d GameMode=%s PlayerControllerClass=%s DefaultPawnClass=%s"),
		*GetWorld()->GetMapName(),
		static_cast<int32>(GetNetMode()),
		*GetNameSafe(this),
		*GetNameSafe(PlayerControllerClass),
		*GetNameSafe(DefaultPawnClass));
}

void ANBGameMode::PreLogin(
	const FString& Options,
	const FString& Address,
	const FUniqueNetIdRepl& UniqueId,
	FString& ErrorMessage)
{
	UE_LOG(LogNBGameMode, Display,
		TEXT("PreLogin request: Address=%s"),
		*Address);

	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	UE_LOG(LogNBGameMode, Display,
		TEXT("PreLogin result: Address=%s Result=%s"),
		*Address,
		ErrorMessage.IsEmpty() ? TEXT("Accepted") : *ErrorMessage);
}

void ANBGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	GetWorldTimerManager().ClearTimer(EmptyServerRestartTimerHandle);

	if (IsValid(NewPlayer))
	{
		APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>();
		PlayerRegistryComponent->AddPlayer(PlayerState);
		UE_LOG(LogNBGameMode, Display,
			TEXT("PostLogin: Controller=%s PlayerState=%s PlayerName=%s Pawn=%s PlayerCount=%d"),
			*GetNameSafe(NewPlayer),
			*GetNameSafe(PlayerState),
			IsValid(PlayerState) ? *PlayerState->GetPlayerName() : TEXT("None"),
			*GetNameSafe(NewPlayer->GetPawn()),
			PlayerRegistryComponent->GetPlayerCount());
		EvaluateGamePhase();
	}
}

void ANBGameMode::Logout(AController* Exiting)
{
	if (IsValid(Exiting))
	{
		APlayerState* ExitingPlayerState = Exiting->GetPlayerState<APlayerState>();
		UE_LOG(LogNBGameMode, Display,
			TEXT("Logout: Controller=%s PlayerState=%s Pawn=%s"),
			*GetNameSafe(Exiting),
			*GetNameSafe(ExitingPlayerState),
			*GetNameSafe(Exiting->GetPawn()));
		APlayerState* NextPlayer = PlayerRegistryComponent->GetNextPlayer(ExitingPlayerState);
		PlayerRegistryComponent->RemovePlayer(ExitingPlayerState);

		if (PlayerRegistryComponent->GetPlayerCount() < MinimumPlayerCount)
		{
			EvaluateGamePhase();
		}
		else
		{
			GameFlowComponent->HandlePlayerLogout(
				ExitingPlayerState,
				NextPlayer);
		}
	}

	Super::Logout(Exiting);

	if (PlayerRegistryComponent->GetPlayerCount() == 0)
	{
		GetWorldTimerManager().SetTimer(
			EmptyServerRestartTimerHandle,
			this,
			&ThisClass::HandleEmptyServerRestart,
			EmptyServerRestartDelay,
			false);
	}
}

void ANBGameMode::RestartPlayer(AController* NewPlayer)
{
	UE_LOG(LogNBGameMode, Display,
		TEXT("RestartPlayer begin: Controller=%s ExistingPawn=%s"),
		*GetNameSafe(NewPlayer),
		*GetNameSafe(IsValid(NewPlayer) ? NewPlayer->GetPawn() : nullptr));

	Super::RestartPlayer(NewPlayer);

	UE_LOG(LogNBGameMode, Display,
		TEXT("RestartPlayer end: Controller=%s SpawnedPawn=%s"),
		*GetNameSafe(NewPlayer),
		*GetNameSafe(IsValid(NewPlayer) ? NewPlayer->GetPawn() : nullptr));
}

void ANBGameMode::ForceStartGame()
{
	GetWorldTimerManager().ClearTimer(GameStartTimerHandle);
	BroadcastGameStartTimer(false);
	GameFlowComponent->ForceStartGameFlow();
}

void ANBGameMode::BroadcastTimerMessage(const FNBTimerMessage& Message)
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator();
		Iterator;
		++Iterator)
	{
		ANBPlayerController* PlayerController = Cast<ANBPlayerController>(Iterator->Get());
		if (IsValid(PlayerController)
			&& IsValid(PlayerController->GetClientMessageComponent()))
		{
			PlayerController->GetClientMessageComponent()->SendTimerMessage(Message);
		}
	}
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
		if (PlayerRegistryComponent->GetPlayerCount() >= MinimumPlayerCount)
		{
			BroadcastWaitingNotification(false);
			if (GetWorldTimerManager().IsTimerActive(GameStartTimerHandle) == false)
			{
				GetWorldTimerManager().SetTimer(
					GameStartTimerHandle,
					this,
					&ThisClass::HandleGameStartDelayExpired,
					GameStartDelay,
					false);
				BroadcastGameStartTimer(true);
			}
			else
			{
				BroadcastGameStartTimer(true);
			}
		}
		else
		{
			BroadcastWaitingNotification(true);
			const bool bWasTimerActive =
				GetWorldTimerManager().IsTimerActive(GameStartTimerHandle);
			GetWorldTimerManager().ClearTimer(GameStartTimerHandle);
			if (bWasTimerActive)
			{
				BroadcastGameStartTimer(false);
			}
		}

		return;
	}

	if (NBGameState->GetCurrentGamePhase() != ENBGamePhase::Finished
		&& PlayerRegistryComponent->GetPlayerCount() < MinimumPlayerCount)
	{
		GetWorldTimerManager().ClearTimer(GameStartTimerHandle);
		GameFlowComponent->ReturnToWaitingForPlayers();
		BroadcastWaitingNotification(true);
	}
}

void ANBGameMode::HandleGameStartDelayExpired()
{
	BroadcastGameStartTimer(false);

	ANBGameState* NBGameState = GetGameState<ANBGameState>();
	if (IsValid(NBGameState)
		&& NBGameState->GetCurrentGamePhase() == ENBGamePhase::WaitingForReady
		&& PlayerRegistryComponent->GetPlayerCount() >= MinimumPlayerCount)
	{
		GameFlowComponent->StartGameFlow();
	}
}

void ANBGameMode::HandleEmptyServerRestart()
{
	if (PlayerRegistryComponent->GetPlayerCount() > 0)
	{
		return;
	}

	const UNetDriver* NetDriver = GetWorld()->GetNetDriver();
	if (IsValid(NetDriver) && NetDriver->ClientConnections.IsEmpty() == false)
	{
		GetWorldTimerManager().SetTimer(
			EmptyServerRestartTimerHandle,
			this,
			&ThisClass::HandleEmptyServerRestart,
			EmptyServerRestartDelay,
			false);
		return;
	}

	RestartGame();
}

void ANBGameMode::BroadcastGameStartTimer(bool bIsActive)
{
	ANBGameState* NBGameState = GetGameState<ANBGameState>();
	if (IsValid(NBGameState) == false)
	{
		return;
	}

	FNBTimerMessage Message;
	Message.TimerType = NBTimerTypes::GameStart;
	Message.Duration = GameStartDelay;
	Message.bIsActive = bIsActive;
	if (bIsActive)
	{
		const float RemainingTime = FMath::Max(
			0.f,
			GetWorldTimerManager().GetTimerRemaining(GameStartTimerHandle));
		Message.EndServerTime =
			NBGameState->GetServerWorldTimeSeconds() + RemainingTime;
	}

	BroadcastTimerMessage(Message);
}

void ANBGameMode::BroadcastWaitingNotification(bool bWaitingForPlayers)
{
	FNBNotifyMessage Message;
	Message.MessageType = bWaitingForPlayers
		? NBNotifyTypes::WaitingForPlayers
		: NBNotifyTypes::WaitingForGameStart;
	Message.Message = bWaitingForPlayers
		? NSLOCTEXT("NumberBaseball", "WaitingForPlayersNotification", "유저 입장 대기중..")
		: NSLOCTEXT("NumberBaseball", "WaitingForGameStartNotification", "게임 시작 대기중");

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator();
		Iterator;
		++Iterator)
	{
		ANBPlayerController* PlayerController = Cast<ANBPlayerController>(Iterator->Get());
		if (IsValid(PlayerController)
			&& IsValid(PlayerController->GetClientMessageComponent()))
		{
			PlayerController->GetClientMessageComponent()->SendNotifyMessage(Message);
		}
	}
}
