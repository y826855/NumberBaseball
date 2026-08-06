#pragma once

#include "GameFramework/GameMode.h"
#include "TimerManager.h"
#include "NBGameMode.generated.h"

class APlayerState;
struct FUniqueNetIdRepl;
class UNBGameFlowComponent;
class UNBPendingTaskComponent;
class UNBPlayerRegistryComponent;
struct FNBTimerMessage;

UCLASS()
class GAMEANIMATIONSAMPLE_API ANBGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ANBGameMode();

	virtual void PreLogin(
		const FString& Options,
		const FString& Address,
		const FUniqueNetIdRepl& UniqueId,
		FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual void RestartPlayer(AController* NewPlayer) override;

	void ForceStartGame();
	void BroadcastTimerMessage(const FNBTimerMessage& Message);
	UNBPendingTaskComponent* GetPendingTaskComponent() const { return PendingTaskComponent; }
	UNBPlayerRegistryComponent* GetPlayerRegistryComponent() const { return PlayerRegistryComponent; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Number Baseball")
	TObjectPtr<UNBGameFlowComponent> GameFlowComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PendingTask")
	TObjectPtr<UNBPendingTaskComponent> PendingTaskComponent;

private:
	void EvaluateGamePhase();
	void HandleGameStartDelayExpired();
	void HandleEmptyServerRestart();
	void BroadcastGameStartTimer(bool bIsActive);
	void BroadcastWaitingNotification(bool bWaitingForPlayers);

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNBPlayerRegistryComponent> PlayerRegistryComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Number Baseball|Game")
	int32 MinimumPlayerCount = 2;

	UPROPERTY(EditDefaultsOnly, Category = "Number Baseball|Game")
	float GameStartDelay = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Number Baseball|Game", meta = (ClampMin = "0.1"))
	float EmptyServerRestartDelay = 1.f;

	FTimerHandle GameStartTimerHandle;
	FTimerHandle EmptyServerRestartTimerHandle;
};
