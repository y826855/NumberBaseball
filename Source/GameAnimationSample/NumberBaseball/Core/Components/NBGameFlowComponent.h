#pragma once

#include "Components/ActorComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "TimerManager.h"
#include "NBGameFlowComponent.generated.h"

class ANBGameState;
struct FGameplayTag;
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
	void StartRound();

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void StartUserInputTurn();

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void EndUserInputTurn();

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void CompleteTurn();

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void CompleteRound();

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void FinishGameFlow();

	void ForceStartGameFlow();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	ANBGameState* GetNBGameState() const;
	void GenerateAnswerNumbers();

	void HandleInputValuesChanged(
		FGameplayTag Channel,
		const FNBInputValuesChangedMessage& Message);

	UPROPERTY(EditDefaultsOnly, Category = "GameFlow", meta = (ClampMin = "1"))
	int32 TotalRoundCount = 3;

	UPROPERTY(EditDefaultsOnly, Category = "GameFlow", meta = (ClampMin = "1", ClampMax = "9"))
	int32 AnswerNumberCount = 3;

	UPROPERTY(EditDefaultsOnly, Category = "GameFlow", meta = (ClampMin = "1.0"))
	float UserInputDuration = 20.f;

	UPROPERTY(Transient)
	TArray<int32> AnswerNumbers;

	int32 CurrentRound = 0;
	bool bLastAnswerCorrect = false;
	FTimerHandle UserInputTimerHandle;
	FGameplayMessageListenerHandle InputValuesChangedListenerHandle;
};
