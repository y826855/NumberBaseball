#pragma once

#include "GameFramework/GameMode.h"
#include "TimerManager.h"
#include "NBGameMode.generated.h"

class APlayerState;
class UNBGameFlowComponent;
class UNBPendingTaskComponent;
class UNBPlayerRegistryComponent;

UCLASS()
class GAMEANIMATIONSAMPLE_API ANBGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ANBGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	void SetPlayerReady(APlayerState* PlayerState, bool bIsReady);
	void ForceStartGame();
	UNBPendingTaskComponent* GetPendingTaskComponent() const { return PendingTaskComponent; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Number Baseball")
	TObjectPtr<UNBGameFlowComponent> GameFlowComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PendingTask")
	TObjectPtr<UNBPendingTaskComponent> PendingTaskComponent;

private:
	void EvaluateGamePhase();
	void HandleGameStartDelayExpired();

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNBPlayerRegistryComponent> PlayerRegistryComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Number Baseball|Game")
	int32 MinimumPlayerCount = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Number Baseball|Game")
	float GameStartDelay = 5.f;

	FTimerHandle GameStartTimerHandle;
};
