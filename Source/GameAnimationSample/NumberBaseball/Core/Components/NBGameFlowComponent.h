#pragma once

#include "Components/ActorComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "TimerManager.h"
#include "NBGameFlowComponent.generated.h"

class ANBGameState;
class APlayerState;
struct FGameplayTag;
struct FNBGuessResult;
struct FNBInputValuesChangedMessage;

UCLASS(ClassGroup = (NumberBaseball), meta = (BlueprintSpawnableComponent))
class GAMEANIMATIONSAMPLE_API UNBGameFlowComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNBGameFlowComponent();

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void StartGameFlow();

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void StartNextRound();

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void StartRound();

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void BeginRound();

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void StartTurn();

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void StartUserInputTurn();

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void EndUserInputTurn();

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void CompleteTurn();

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void CompleteRound();

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void CompleteGameFlow();

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void FinishGameFlow();

	void ReturnToWaitingForPlayers();
	void HandlePlayerLogout(APlayerState* ExitingPlayer);

	void ForceStartGameFlow();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	using FFlowTimerCallback = void (UNBGameFlowComponent::*)();

	ANBGameState* GetNBGameState() const;
	void ScheduleFlowStep(float Delay, FFlowTimerCallback Callback);
	void ClearFlowTimer();
	void SendUserInputTimer(bool bIsTimerActive) const;
	void SendTurnNotifications(bool bIsTurnActive) const;
	void ResetPlayerScores() const;
	bool SelectRoundStartingPlayer();
	bool AdvanceTurnPlayer();
	void GenerateAnswerNumbers();
	FNBGuessResult BuildGuessResult(const TArray<int32>& InputValues);

	void HandleInputValuesChanged(
		FGameplayTag Channel,
		const FNBInputValuesChangedMessage& Message);

	UPROPERTY(EditDefaultsOnly, Category = "GameFlow", meta = (ClampMin = "1"))
	int32 TotalRoundCount = 3;

	UPROPERTY(EditDefaultsOnly, Category = "GameFlow", meta = (ClampMin = "1", ClampMax = "9"))
	int32 AnswerNumberCount = 3;

	UPROPERTY(EditDefaultsOnly, Category = "GameFlow", meta = (ClampMin = "1.0"))
	float UserInputDuration = 20.f;

	UPROPERTY(EditDefaultsOnly, Category = "GameFlow", meta = (ClampMin = "0.1"))
	float RoundStartingDuration = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "GameFlow", meta = (ClampMin = "0.1"))
	float StartingTurnDuration = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "GameFlow", meta = (ClampMin = "0.1"))
	float ResultPresentationMaxWaitTime = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "GameFlow", meta = (ClampMin = "0.1"))
	float RoundEndingDuration = 1.f;

	UPROPERTY(Transient)
	TArray<int32> AnswerNumbers;

	int32 CurrentRound = 0;
	int32 NextAttemptId = 0;
	bool bLastAnswerCorrect = false;
	FTimerHandle FlowTimerHandle;
	FGameplayMessageListenerHandle InputValuesChangedListenerHandle;
};
